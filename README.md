# account-migration
migration tool for sync-monitor to buteo caldav.
For buteo to work ( profile creation at account update ), msyncd service need to be up and running

usage example:

./account-migration --old-service=nextcloud-caldav --new-service=lomiri-nextcloud-caldav --settings=webdav_path=/remote.php/dav

./account-migration --old-service=owncloud-caldav --new-service=lomiri-owncloud-caldav --settings=webdav_path=/remote.php/webdav

./account-migration --old-service=generic-caldav --new-service=lomiri-generic-caldav --settings=webdav_path=/

