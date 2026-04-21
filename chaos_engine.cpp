#include "chaos_engine.h"

ChaosEngine::ChaosEngine(char (&front_to_back_buff)[], char (&back_to_front_buff)[], bool& request, bool& response): request(request), response(response){
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

void ChaosEngine::chunk_drop(){ // https://en.cppreference.com/cpp/numeric/random/bernoulli_distribution
    if (!j.at("chaos").at("chunk_drop").at("enabled")){return;}
    random_device rd;
    mt19937 gen(rd());

    bernoulli_distribution d(j.at("chaos").at("chunk_drop").at("rate"));

    if(j.at("chaos").at("chunk_drop").at("direction") == "both"){
        if (d(gen) == 1){
            strcpy(front_to_back_buff, "");
            strcpy(back_to_front_buff, ""); //TODO CHECK IF YOU NEED TO CHANGE OFFSETS and BYTES
        }
        return;
    }

    if(request){ // front sent data

    }else if (response){ //received data from back

    }else{

    }
}
void ChaosEngine::directional_drop(){}


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
    for(auto& i : registry){
        registry[i.first]();
    }
}