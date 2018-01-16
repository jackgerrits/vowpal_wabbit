#include "ds_predictors.h"

#include 

namespace Microsoft {
  namespace DecisionService {
      using namespace std;

        void DecisionServicePrediction::set(const std::vector<float>& scores)
        {
            _scores = scores;
        }

        size_t DecisionServicePrediction::top_action() const
        {
            return std::max_element(_scores.begin(), _scores.end()) - _scores.begin();
        }

        float DecisionServicePrediction::max_score() const
        {
            return _scores[top_action()];
        }

        size_t DecisionServicePrediction::num_actions() const
        {
            return _scores.size();
        }

        std::vector<float>& DecisionServicePrediction::scores()
        {
            return _scores;
        }

      DecisionServicePredictors::DecisionServicePredictors() : DecisionServicePredictors(1)
      { }

      DecisionServicePredictors(size_t count)
        : _count(count)
      { }
      
      DecisionServicePredictors::~DecisionServicePredictors()
      { }

      void DecisionServicePredictors::get_prediction(size_t index, const std::vector<int>& previous_decisions, DecisionServicePredictionResult* output_result)
      { 
        // TODO: throw Exception()
        // this should be virtual, but doesn't play well with swig           
      }
  }
}
  