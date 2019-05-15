
void auditorbos::updatebio(name cand, string bio) {
    require_auth(cand);
    const auto &reg_candidate = registered_candidates.get(cand.value, "ERR::UPDATEBIO_NOT_CURRENT_REG_CANDIDATE::Candidate is not already registered.");

    auto cand_bio = candidate_bios.find(cand.value);

    if(bio.size() > 0) {
        if (cand_bio != candidate_bios.end()) {
            candidate_bios.modify(cand_bio, cand, [&](auto &b) {
                b.bio = bio;
            });
        } else {
            candidate_bios.emplace(cand, [&](auto &b) {
                b.candidate_name = cand;
                b.bio = bio;
            });
        }
    } else {
        if (cand_bio != candidate_bios.end()) {
            candidate_bios.erase(cand_bio);
        }
    }
}
