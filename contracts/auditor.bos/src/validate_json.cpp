#define VALIDATE_JSON(Variable, MAX_SIZE)\
::auditorbos::validate_json(\
    Variable,\
    MAX_SIZE,\
    #Variable " must be a JSON object (if specified).",\
    #Variable " should be shorter than " #MAX_SIZE " bytes."\
)

// Do not use directly, use the VALIDATE_JSON macro instead!
void auditorbos::validate_json(
    const string& payload,
    size_t max_size,
    const char* not_object_message,
    const char* over_size_message
) {
    if (payload.size() <= 0) return;

    check(payload[0] == '{', not_object_message);
    check(payload.size() < max_size, over_size_message);
}
