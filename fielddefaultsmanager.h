#ifndef FIELDDEFAULTSMANAGER_H
#define FIELDDEFAULTSMANAGER_H

#include <QString>
#include <QMap>
#include <QJsonObject>

class FieldDefaultsManager
{
public:
    static FieldDefaultsManager& instance();
    
    // Save a field default value
    void saveFieldDefault(const QString &fieldName, const QString &value);
    
    // Get a field default value
    QString getFieldDefault(const QString &fieldName) const;
    
    // Check if a field has a default value
    bool hasFieldDefault(const QString &fieldName) const;
    
    // Save all defaults to file
    bool saveDefaults() const;
    
    // Load defaults from file
    bool loadDefaults();
    
private:
    FieldDefaultsManager();
    ~FieldDefaultsManager();
    
    // Prevent copying
    FieldDefaultsManager(const FieldDefaultsManager&) = delete;
    FieldDefaultsManager& operator=(const FieldDefaultsManager&) = delete;
    
    // Map of field names to default values
    QMap<QString, QString> m_defaults;
    
    // Path to the defaults file
    QString m_defaultsFilePath;
    
    // Convert to/from JSON
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
};

#endif // FIELDDEFAULTSMANAGER_H
