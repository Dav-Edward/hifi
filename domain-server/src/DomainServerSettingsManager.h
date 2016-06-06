//
//  DomainServerSettingsManager.h
//  domain-server/src
//
//  Created by Stephen Birarda on 2014-06-24.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_DomainServerSettingsManager_h
#define hifi_DomainServerSettingsManager_h

#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>

#include <HifiConfigVariantMap.h>
#include <HTTPManager.h>

#include <ReceivedMessage.h>
#include "NodePermissions.h"

const QString SETTINGS_PATHS_KEY = "paths";

const QString SETTINGS_PATH = "/settings";
const QString SETTINGS_PATH_JSON = SETTINGS_PATH + ".json";
const QString AGENT_STANDARD_PERMISSIONS_KEYPATH = "security.standard_permissions";
const QString AGENT_PERMISSIONS_KEYPATH = "security.permissions";

class DomainServerSettingsManager : public QObject {
    Q_OBJECT
public:
    DomainServerSettingsManager();
    bool handlePublicHTTPRequest(HTTPConnection* connection, const QUrl& url);
    bool handleAuthenticatedHTTPRequest(HTTPConnection* connection, const QUrl& url);

    void setupConfigMap(const QStringList& argumentList);
    QVariant valueOrDefaultValueForKeyPath(const QString& keyPath);

    QVariantMap& getUserSettingsMap() { return _configMap.getUserConfig(); }
    QVariantMap& getSettingsMap() { return _configMap.getMergedConfig(); }

    bool haveStandardPermissionsForName(const QString& name) const { return _standardAgentPermissions.contains(name); }
    bool havePermissionsForName(const QString& name) const { return _agentPermissions.contains(name); }
    NodePermissions getStandardPermissionsForName(const QString& name) const;
    NodePermissions getPermissionsForName(const QString& name) const;
    QStringList getAllNames() { return _agentPermissions.keys(); }

private slots:
    void processSettingsRequestPacket(QSharedPointer<ReceivedMessage> message);

private:
    QJsonObject responseObjectForType(const QString& typeValue, bool isAuthenticated = false);
    void recurseJSONObjectAndOverwriteSettings(const QJsonObject& postedObject);

    void updateSetting(const QString& key, const QJsonValue& newValue, QVariantMap& settingMap,
                       const QJsonObject& settingDescription);
    QJsonObject settingDescriptionFromGroup(const QJsonObject& groupObject, const QString& settingName);
    void persistToFile();

    double _descriptionVersion;
    QJsonArray _descriptionArray;
    HifiConfigVariantMap _configMap;

    friend class DomainServer;

    void packPermissionsForMap(const QStringList& argumentList, QString mapName,
                               QHash<QString, NodePermissionsPointer> agentPermissions, QString keyPath);
    void packPermissions(const QStringList& argumentList);
    void unpackPermissions(const QStringList& argumentList);
    QHash<QString, NodePermissionsPointer> _standardAgentPermissions; // anonymous, logged-in, localhost
    QHash<QString, NodePermissionsPointer> _agentPermissions; // specific account-names
};

#endif // hifi_DomainServerSettingsManager_h
