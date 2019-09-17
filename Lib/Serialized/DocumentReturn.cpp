#include "DocumentReturn.hpp"

#include "Document.hpp"

namespace acid {
DocumentReturn::DocumentReturn(Document const *parent, std::variant<std::string, int32_t> key, Document const *value) :
	m_parent(const_cast<Document *>(parent)),
	m_keys{std::move(key)},
	m_value(const_cast<Document *>(value)) {
}

DocumentReturn::DocumentReturn(DocumentReturn *parent, std::variant<std::string, int32_t> key) :
	m_parent(parent->m_parent),
	m_keys(parent->m_keys) {
	m_keys.emplace_back(std::move(key));
}

bool DocumentReturn::has_value() const noexcept {
	return m_value != nullptr;
}

Document *DocumentReturn::get() {
	if (!has_value()) {
		// This will build the tree of nodes from the return keys tree.
		for (const auto &key : m_keys) {
			if (std::holds_alternative<std::string>(key)) {
				const auto &name = std::get<std::string>(key);
				m_value = &m_parent->AddProperty(name, {});
			} else if (std::holds_alternative<int32_t>(key)) {
				const auto &index = std::get<std::int32_t>(key);
				m_value = &m_parent->AddProperty(index, {});
			} else {
				throw std::runtime_error("Key for node return is neither a int or a string");
			}

			// Because the last key will set parent to the value parent usage should be avoided.
			m_parent = m_value;
		}

		m_keys.erase(m_keys.begin(), m_keys.end() - 1);
	}

	return m_value;
}

DocumentReturn DocumentReturn::operator[](const std::string &key) {
	if (!has_value()) {
		return {this, key};
	}

	return get()->operator[](key);
}

DocumentReturn DocumentReturn::operator[](uint32_t index) {
	if (!has_value()) {
		return {this, index};
	}

	return get()->operator[](index);
}

std::string DocumentReturn::GetName() const {
	if (!has_value()) {
		return *std::get_if<std::string>(&m_keys.back());
	}

	return m_value->GetName();
}

void DocumentReturn::SetName(const std::string &name) {
	if (!has_value()) {
		m_keys.back() = name;
		return;
	}

	m_value->SetName(name);
}
}