/**
 * @brief  Android Directory interface
 * @author Kang Lin (kl222@!26.com) 
 */

#ifndef __ADNROID_DIRECTORY__
#define __ADNROID_DIRECTORY__

#pragma once

#include <QObject>

class Q_CORE_EXPORT CAndroidDirectory : public QObject
{
    Q_OBJECT
    
public:
    CAndroidDirectory(QObject *parent = nullptr);
    virtual ~CAndroidDirectory();
    
    static QString GetExternalStorageDirectory();
    static QString GetDataDirectory();
    static QString GetPictureDirectory();
    static QString GetMusicDirectory();
    static QString GetMoviesDirectory();
    static QString GetRingtonesDirectory();
    static QString GetAppFilesDirectory();
    static QString GetAppCacheDirectory();
    static QString GetApkPath();
    
private:
    static QString GetExternalStoragePublicDirectory(QString name);
};

#endif // __ADNROID_DIRECTORY__
