#include"Renderer/Core/uuid.h"

const std::string& uuid::get() const {
    return m_UID;
}

bool uuid::isNull() const {
    return m_UID.empty();
}

uuid uuid::Build() {
    std::random_device rd;
    uint64_t high = ((uint64_t)rd() << 32) | rd();
    uint64_t low = ((uint64_t)rd() << 32) | rd();

    std::stringstream ss;
    ss << std::hex << std::setfill('0')
        << std::setw(16) << high
        << std::setw(16) << low;

    uuid id{};
    id.assign(ss.str());
    return id;
}

void uuid::assign(const std::string& id) {
    m_UID = id;
}