#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <Accounts/Manager>
#include <Accounts/Service>
#include <Accounts/Account>
#include <Accounts/AccountService>


bool migrateAccount(const QString& oldServiceName, const QString& newServiceName, const QMap<QString, QString> &additionalSettings) {

    Accounts::Manager manager;

    Accounts::Service oldService = manager.service(oldServiceName);
    if (!oldService.isValid()) {
        qDebug() << "old service " << oldServiceName << "not found, nothing to do";
        return true;
    }

    Accounts::Service newService = manager.service(newServiceName);
    if (!newService.isValid()) {
        qDebug() << "new service " << newServiceName << "not found";
        return false;
    }

    Accounts::AccountIdList accountIds = manager.accountList(oldService.serviceType());
    if (accountIds.size() == 0) {
        qDebug() << " no accounts associated with service type" << oldService.serviceType() << " nothing to do";
        return true;
    }

    for (Accounts::AccountId accountId : accountIds) {
        Accounts::Account *account = manager.account(accountId);
        if (account) {
            qDebug() << "Checking account:" << account->displayName() << "enabled" << account->isEnabled();

            // old service not found, continue loop
            if (!account->enabledServices().contains(oldService)) {
                qDebug() << " account does not have service" << oldServiceName << " , skip it";
                continue;
            }

            if (!account->enabledServices().contains(newService)) {

                // we store service settings in the global account object
                Accounts::AccountService global(account, Accounts::Service());
                QVariant host = global.value("host");

                // buteo caldav need those values
                global.setValue("server_address", host);
                for (auto it = additionalSettings.begin(); it != additionalSettings.end(); ++it) {
                    global.setValue(it.key(), QVariant::fromValue(it.value()));
                    qDebug() << "Added setting:" << it.key() << "=" << it.value();
                }

                // enable the new one
                account->selectService(newService);
                account->setEnabled(true);
            }

            // now disable oldService
            account->selectService(oldService);
            if (account->isEnabled()) {
                account->setEnabled(false);
            }

            // save it
            account->sync();

            qDebug() << "Account " << account->displayName() << "with old service " << oldServiceName << " migrated to " << newServiceName;

        }
    }

    return true;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    parser.setApplicationDescription("Account Migration");
    parser.addHelpOption();

    parser.addOption({ "old-service", "Old service name", "name" });
    parser.addOption({ "new-service", "New service name", "name" });
    parser.addOption({ "settings", "Additional settings in the form key=value", "key=value", "" });

    parser.process(app);

    QString oldServiceName = parser.value("old-service");
    QString newServiceName = parser.value("new-service");
    QStringList settings = parser.values("settings");

    if (oldServiceName.isEmpty() || newServiceName.isEmpty()) {
        qCritical() << "Both --old-service and --new-service options are required.";
        return EXIT_FAILURE;
    }

    // --old-service=nextcloud-caldav --new-service=lomiri-nextcloud-caldav --settings=webdav_path=/remote.php/dav

    QMap<QString, QString> additionalSettings;
    for (const QString &setting : settings) {
        QStringList keyValue = setting.split('=');
        if (keyValue.size() == 2) {
            additionalSettings.insert(keyValue[0], keyValue[1]);
        } else {
            qWarning() << "Invalid setting format:" << setting;
        }
    }

    bool ok = migrateAccount(oldServiceName, newServiceName, additionalSettings);

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
