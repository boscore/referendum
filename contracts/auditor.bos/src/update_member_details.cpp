#include "validate_json.cpp"

void auditorbos::updatebio(name cand, string bio) {
    require_auth(cand);
    VALIDATE_JSON(bio, 32768);

    const auto &reg_candidate = registered_candidates.get(cand.value, "ERR::UPDATEBIO_NOT_CURRENT_REG_CANDIDATE::Candidate is not already registered.");

    auto cand_bio = candidate_bios.find(cand.value);

    if(bio.size() > 0) {
        if (cand_bio != candidate_bios.end()) {
            candidate_bios.modify(cand_bio, cand, [&](auto & row) {
                row.bio = bio;
            });
        } else {
            candidate_bios.emplace(cand, [&](auto & row) {
                row.candidate_name = cand;
                row.bio = bio;
            });
        }
    } else {
        if (cand_bio != candidate_bios.end()) {
            candidate_bios.erase(cand_bio);
        }
    }
}
