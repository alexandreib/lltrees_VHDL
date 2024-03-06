#include "conf.hpp"
#include "gbt.hpp"
#include "metrics.hpp"
#include "criterion.hpp"
#include "wrapper.hpp"

/// Need to work on abtract 
Gbt* gbt_Factory();
std::unique_ptr<data> data_Factory();
std::shared_ptr<metrics> metric_Factory();
std::shared_ptr<criterion> criterion_Factory();
