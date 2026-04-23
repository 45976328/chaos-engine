#include "chaos_engine.h"

ChaosEngine::ChaosEngine(char (&front_to_back_buff)[], char (&back_to_front_buff)[], bool& request, bool& response, int& ftbbytes, int& ftb_offset, int& btfbytes, int& btf_offset)
    : request(request), response(response), ftbbytes(ftbbytes), ftb_offset(ftb_offset), btfbytes(btfbytes), btf_offset(btf_offset){
    
    this->front_to_back_buff = front_to_back_buff;
    this->back_to_front_buff =  back_to_front_buff;
    
    try{
        f.open("config.json");
        j = json::parse(f);
        f.close();
    }catch(exception& e){
        cerr << e.what() << endl;
    }
    register_all();
}

ChaosEngine::~ChaosEngine(){}

void ChaosEngine::chunk_drop(){}

void ChaosEngine::directional_drop(){
    // https://en.cppreference.com/cpp/numeric/random/bernoulli_distribution
    if (!j.at("chaos").at("directional_drop").at("enabled")){return;}
    
    bernoulli_distribution d(j.at("chaos").at("directional_drop").at("rate"));

    if(request && j.at("chaos").at("directional_drop").at("direction") == "request"){ // front sent data
        if (d(gen) == 1){
            // cout << "request: Triggered Directional Drop " << request <<endl;
            ftbbytes = 0;
            ftb_offset = 0;
        }
    }else if (response && j.at("chaos").at("directional_drop").at("direction") == "response"){ //received data from back
        if (d(gen) == 1){
            // cout << "response: Triggered Directional Drop " << response << endl;
            btfbytes = 0;
            btf_offset = 0;
        }
    }
}


void ChaosEngine::byte_corruption(){}
void ChaosEngine::bit_flipping(){}
void ChaosEngine::header_corruption(){}
void ChaosEngine::body_corruption(){}

void ChaosEngine::fixed_delay(){}
void ChaosEngine::random_delay(){}


void ChaosEngine::header_removal(){}
void ChaosEngine::header_duplication(){}
void ChaosEngine::wrong_content_length(){}
void ChaosEngine::status_manipulation(){}

void ChaosEngine::logs(){}

void ChaosEngine::register_all(){ //TODO lookup Static Auto-Registration 
        registry["chunk_drop"] = [this]() { chunk_drop(); };
        registry["directional_drop"] = [this]() { directional_drop(); };
        registry["byte_corruption"] = [this]() { byte_corruption(); };
        registry["bit_flipping"] = [this]() { bit_flipping(); };
        registry["fixed_delay"] = [this]() { fixed_delay(); };

        for (auto& i : j.at("chaos").items()){
            if(registry.count(i.key()) < 1){
                registry.erase(i.key());
            }
        }
}
void ChaosEngine::dispatch(){
    if(!j.at("chaos").at("enabled")){return;}
    for(auto& i : registry){
        registry[i.first]();
    }
}