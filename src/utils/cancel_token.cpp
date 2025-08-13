#include "utils/cancel_token.hpp"

// Definition of static members
std::vector<CancelToken*> CancelToken::instances;
std::mutex CancelToken::instancesMutex;

CancelToken::CancelToken() {
    std::lock_guard<std::mutex> lock(instancesMutex);
    instances.push_back(this);
}

CancelToken::~CancelToken() {
    std::lock_guard<std::mutex> lock(instancesMutex);
    auto it = std::find(instances.begin(), instances.end(), this);
    if (it != instances.end()) {
        instances.erase(it);
    }
}

bool CancelToken::isCancelled() const {
    return cancelled;
}

void CancelToken::cancel() {
    std::lock_guard<std::mutex> lock(instancesMutex);
    cancelled = true;
}

void CancelToken::cancelAll() {
    std::lock_guard<std::mutex> lock(instancesMutex);
    for (auto* token : instances) {
        if (token) {
            token->cancelled = true;
        }
    }
}