#include "chaos_engine.h"

ChaosEngine::ChaosEngine(){}
ChaosEngine::~ChaosEngine(){}

void ChaosEngine::chunk_drop(bool enabled, int rate, string mode, string direction){
    if(!enabled){return;}


//     {
//   "chaos": {
//     "chunk_drop": {
//       "enabled": true,
//       "rate": 0.05,
//       "direction": "request",
//       "mode": "full"
//     }
//   }
// }
}

void ChaosEngine::byte_corruption(bool enabled, int rate, string mode, string direction){
    if (!enabled){return;}



//     {
//   "chaos": {
//     "byte_corruption": {
//       "enabled": true,
//       "rate": 0.05,
//       "bytes_per_message": 2,
//       "mode": "body_only",
//       "direction": "both"
//     }
//   }
// }
}

void ChaosEngine::directional_drop(){}
void ChaosEngine::fixed_delay(){}
void ChaosEngine::random_delay(){}
void ChaosEngine::directional_latency(){}
void ChaosEngine::chunk_split(){}
void ChaosEngine::early_close(){}
void ChaosEngine::wrong_content_length(){}