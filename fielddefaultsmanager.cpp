#include "fielddefaultsmanager.h"
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QStandardPaths>

FieldDefaultsManager::FieldDefaultsManager()
{
    // Set up the defaults file path
    m_defaultsFilePath = QDir::homePath() + "/.llvm_builder.conf";
    
    // Load defaults from file
    loadDefaults();
}

FieldDefaultsManager::~FieldDefaultsManager()
{
    // Save defaults when the manager is destroyed
    saveDefaults();
}

FieldDefaultsManager& FieldDefaultsManager::instance()
{
    static FieldDefaultsManager instance;
    return instance;
}

void FieldDefaultsManager::saveFieldDefault(const QString &fieldName, const QString &value)
{
    // Save the default value
    m_defaults[fieldName] = value;
    
    // Save to file
    saveDefaults();
}

QString FieldDefaultsManager::getFieldDefault(const QString &fieldName) const
{
    // Return the default value if it exists
    if (m_defaults.contains(fieldName)) {
        return m_defaults[fieldName];
    }
    
    // Return an empty string if no default exists
    return QString();
}

bool FieldDefaultsManager::hasFieldDefault(const QString &fieldName) const
{
    return m_defaults.contains(fieldName);
}

bool FieldDefaultsManager::saveDefaults() const
{
    QFile file(m_defaultsFilePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonDocument doc(toJson());
    file.write(doc.toJson());
    return true;
}

bool FieldDefaultsManager::loadDefaults()
{
    QFile file(m_defaultsFilePath);
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull() || !doc.isObject()) {
        return false;
    }
    
    fromJson(doc.object());
    return true;
}

QJsonObject FieldDefaultsManager::toJson() const
{
    QJsonObject json;
    
    // Add all defaults to the JSON object
    for (auto it = m_defaults.constBegin(); it != m_defaults.constEnd(); ++it) {
        json[it.key()] = it.value();
    }
    
    return json;
}

void FieldDefaultsManager::fromJson(const QJsonObject &json)
{
    // Clear existing defaults
    m_defaults.clear();
    
    // Load defaults from JSON
    for (auto it = json.constBegin(); it != json.constEnd(); ++it) {
        m_defaults[it.key()] = it.value().toString();
    }
}
