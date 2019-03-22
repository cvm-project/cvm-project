int GenerateExecutable(const char* conf, const char* dagstr);  // NOLINT
void FreeResult(const char* s);                                // NOLINT
const char* ExecutePlan(int plan_id, const char* input_str);   // NOLINT
void ClearPlans();                                             // NOLINT
