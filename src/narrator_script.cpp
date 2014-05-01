/*
 * Tell a story with some lights.
 *
 * (c) 2014 Micah Elizabeth Scott
 * http://creativecommons.org/licenses/by/3.0/
 */

#include "lib/sampler.h"
#include "narrator.h"
#include "chaos_particles.h"
#include "order_particles.h"
#include "precursor.h"
#include "rings.h"
#include "partner_dance.h"
#include "glowpoi.h"
#include "explore.h"
#include "forest.h"


int Narrator::script(int st, PRNG &prng)
{
    static ChaosParticles chaosA(flow, runner.config["chaosParticles"]);
    static ChaosParticles chaosB(flow, runner.config["chaosParticles"]);
    static OrderParticles orderParticles(flow, runner.config["orderParticles"]);
    static Precursor precursor(flow, runner.config["precursor"]);
    static RingsEffect ringsA(flow, runner.config["ringsA"]);
    static RingsEffect ringsB(flow, runner.config["ringsB"]);
    static PartnerDance partnerDance(flow, runner.config["partnerDance"]);
    static CameraFlowDebugEffect flowDebugEffect(flow, runner.config["flowDebugEffect"]);
    static GlowPoi glowPoi(flow, runner.config["glowPoi"]);
    static Explore explore(flow, runner.config["explore"]);
    static Forest forest(flow, runner.config["forest"]);

    rapidjson::Value& config = runner.config["narrator"];
    Sampler s(prng.uniform32());

    switch (st) {

        //////////////////////////////////////////////////////////////////////////////////////////////
        // Defaults

        default: {
            return 0;
        }

        case 0: {
            return 10;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////
        // Debug states

        case 1: {
            // Special state; precursor only (sleep mode)            
            precursor.reseed(prng.uniform32());
            crossfade(&precursor, 1);
            delayForever();
        }

        case 2: {
            // Debugging the computer vision system
            crossfade(&flowDebugEffect, 1);
            delayForever();
        }

        case 3: {
            // Tree growth only
            precursor.treeGrowth.reseed(prng.uniform32());
            precursor.treeGrowth.launch(Vec3(0,0,0), Vec3(0,0,0));
            crossfade(&precursor.treeGrowth, 1);
            delayForever();
        }

        case 4: {
            // Agency, creative energy.
            glowPoi.reseed(prng.uniform32());
            crossfade(&glowPoi, 1);
            delayForever();
        }

        case 5: {
            // Explore
            explore.reseed(prng.uniform32());
            crossfade(&explore, 1);
            delayForever();
        }

        case 6: {
            // Work in progress. Seeing through the trees.
            forest.reseed(prng.uniform32());
            crossfade(&forest, 1);
            delayForever();
        }

        //////////////////////////////////////////////////////////////////////////////////////////////
        // Normal states

        case 10: {
            // Order trying to form out of the tiniest sparks; runs for an unpredictable time, fails.
            precursor.reseed(prng.uniform32());
            crossfade(&precursor, s.value(config["precursorCrossfade"]));

            // Bootstrap
            delay(s.value(config["precursorBootstrap"]));

            // Wait for darkness
            while (!precursor.isDone) {
                doFrame();
            }
            return 20;
        }

        case 20: {
            // Bang. Explosive energy, hints of self-organization

            ChaosParticles *pChaosA = &chaosA;
            ChaosParticles *pChaosB = &chaosB;
            
            int bangCount = s.value(config["bangCount"]);
            for (int i = 0; i < bangCount; i++) {
                pChaosA->reseed(prng.circularVector() * 0.6, prng.uniform32());
                crossfade(pChaosA, s.value(config["bangCrossfadeDuration"]));
                delay((1 << i) * s.value(config["bangDelayBasis"]));
                std::swap(pChaosA, pChaosB);
            }

            attention(s, config["bangAttention"]);

            return 30;
        }

        case 30: {
            // Textures of light, exploring something formless. Slow crossfade in
            ringsA.reseed();
            crossfade(&ringsA, s.value(config["ringsA-Crossfade"]));
            attention(s, config["ringsA-Attention"]);
            return 40;
        }

        case 40: {
            // Add energy, explore another layer.
            ringsB.reseed();
            crossfade(&ringsB, s.value(config["ringsB-Crossfade"]));
            attention(s, config["ringsB-Attention"]);
            return 50;
        }

        case 50: {
            // Biology happens, order emerges. Cellular look, emergent order.

            orderParticles.reseed(prng.uniform32());
            orderParticles.symmetry = 10;
            crossfade(&orderParticles, s.value(config["orderCrossfade"]));
            while (orderParticles.symmetry > 4) {
                attention(s, config["orderStepAttention"]);
                orderParticles.symmetry--;
            }
            attention(s, config["orderStepAttention"]);
            return 60;
        }

        case 60: {
            // Two partners, populations of particles.
            // Spiralling inwards. Depression. Beauty on the edge of destruction,
            // pressing forward until nothing remains.

            partnerDance.reseed(prng.uniform32());
            crossfade(&partnerDance, s.value(config["partnerCrossfade"]));
            attention(s, config["partnerAttention"]);
            return 70;
        }
    }
}
