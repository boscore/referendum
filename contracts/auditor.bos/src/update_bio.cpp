#include "validate_json.cpp"

void auditorbos::updatebio( const name cand, const string bio ) {
    require_auth( cand );
    VALIDATE_JSON( bio, 32768 );

    const auto &reg_candidate = _candidates.get(cand.value, "ERR::UPDATEBIO_NOT_CURRENT_REG_CANDIDATE::Candidate is not already registered.");

    auto cand_bio = _bios.find(cand.value);

    if(bio.size() > 0) {
        if (cand_bio != _bios.end()) {
            _bios.modify(cand_bio, cand, [&](auto & row) {
                row.bio = bio;
            });
        } else {
            _bios.emplace(cand, [&](auto & row) {
                row.candidate_name = cand;
                row.bio = bio;
            });
        }
    } else {
        if (cand_bio != _bios.end()) {
            _bios.erase(cand_bio);
        }
    }
}
