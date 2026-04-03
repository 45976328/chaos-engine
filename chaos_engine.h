#ifndef CHAOS_ENGINE_H
#define CHAOS_ENGINE_H

#include <iostream>
#include <string>

using namespace std;

class ChaosEngine{
    public:
        ChaosEngine();
        ~ChaosEngine();


        // LOSS CHAOS
        void chunk_drop(bool, int, string, string);
        void directional_drop();
        void probabilistic_drop();
        void burst_loss();
        void selective_drop();

        //CORRUPTION CHAOS
        void byte_corruption(bool, int, string, string);
        void bit_flipping();
        void header_corruption();
        void body_corruption();
        void structured_corruption();

        //TIMING/LATENCY CHAOS
        void fixed_delay();
        void random_delay();
        void jitter();
        void directional_latency();
        void delay_first_byte();
        void tail_latency_injection();

        //BANDWIDTH / THROUGHPUT SHAPING
        void rate_limit();
        void chunk_split();
        void fragment();

        //CONNECTION-LEVEL FAULTS
        void early_close();
        void ide_timeout_kill();
        void connection_refusal();
        void connect_delay();
        void half_close();
        void force_reset();

        //PROTOCOL-AWARE FAULTS
        void malform_request();
        void header_removal();
        void header_duplication();
        void wrong_content_length();
        void truncate_body();
        void invalid_crlf_format();
        void status_manipulation();

        //TRAFFIC SELECTION / TARGETING
        //Direction filters
        //Request path filters
        //Method filters
        //Header-based filters

        //OBSERVABILITY / CONTROL / REPLAY
        void logs();

        // Fault counters / metrics
        // Track:

        // number of drops
        // number of corruptions
        // number of delayed events
        // number of forced closes

        // Deterministic random seed

        // Use a seed for your RNG.

        // This means:

        // same chaos run can be reproduced exactly

        // This is extremely useful.

};

#endif