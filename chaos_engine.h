#ifndef CHAOS_ENGINE_H
#define CHAOS_ENGINE_H

#include <iostream>
#include <cstring>
#include <chrono>
#include <fstream>
#include <unordered_map>
#include <random>

#include "nlohmann/json.hpp"

#define BUFFER_SIZE 4096

using namespace std;
using json = nlohmann::json;

class ChaosEngine{
    private:
        char* front_to_back_buff;
        char* back_to_front_buff;
        ifstream f;
        json j;
        unordered_map <string,function<void()>> registry;

        random_device rd;
        mt19937 gen{rd()};

        const bool& request;
		const bool& response;

        int& ftbbytes;
		int& ftb_offset;
        int& btfbytes;
		int& btf_offset;

    public:
        ChaosEngine(char (&)[], char (&)[], bool&, bool&, int&, int&, int&, int&);
        ~ChaosEngine();

        void register_all();
        void dispatch();

        // LOSS CHAOS
        void chunk_drop();
        void directional_drop();
        // void probabilistic_drop();
        // void burst_loss();
        // void selective_drop();

        //CORRUPTION CHAOS
        void byte_corruption();
        void bit_flipping();
        void header_corruption();
        void body_corruption();
        // void structured_corruption();

        //TIMING/LATENCY CHAOS
        void fixed_delay();
        void random_delay();
        void jitter();
        // void directional_latency();
        // void delay_first_byte();
        // void tail_latency_injection();

        //BANDWIDTH / THROUGHPUT SHAPING
        // void rate_limit();
        void chunk_split();
        // void fragment();

        //CONNECTION-LEVEL FAULTS
        void early_close();
        void ide_timeout_kill();
        void connection_refusal();
        void connect_delay();
        void half_close();
        void force_reset();

        //PROTOCOL-AWARE FAULTS
        // void malform_request();
        void header_removal();
        void header_duplication();
        void wrong_content_length();
        // void truncate_body();
        // void invalid_crlf_format();
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

};

#endif