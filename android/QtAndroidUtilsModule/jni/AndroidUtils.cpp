/* Java util
 * Author: KangLin(kl222@!26.com) 
 */

#include "AndroidUtils.h"
#include "ActivityResultReceiver.h"
#include <QtAndroidExtras>
#include <QtAndroid>
#if (QT_VERSION > QT_VERSION_CHECK(5, 10, 0))
    #include <QAndroidIntent>
    #include <QAndroidJniExceptionCleaner>
#endif

#define CHECK_EXCEPTION() \
    if(env->ExceptionCheck())\
    {\
    qDebug() << __FILE__ << "(" << __LINE__ << ")" << "exception occured";\
    env->ExceptionClear(); \
    }


CAndroidUtils::CAndroidUtils(QObject *parent) :  QObject(parent)
{
    m_pResultReceiver = new CActivityResultReceiver(this);
    
}

CAndroidUtils::~CAndroidUtils()
{
    if(m_pResultReceiver)
        delete m_pResultReceiver;
}

int CAndroidUtils::InitPermissions()
{
    int nRet = 0;
    static bool inited = false;
    if(inited)
        return 0;
    inited = true;
    nRet = InitExternalStoragePermissions();
    nRet = InitCameraPermissions();
    return nRet;
}

int CAndroidUtils::InitExternalStoragePermissions()
{
    int nRet = 0;
    QAndroidJniEnvironment env;
    
    /*
     The following permission must be set in AndroidManifest.xml:
     <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
     <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
    */
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    QStringList lstPermission;
    QtAndroid::PermissionResult r;
    r = QtAndroid::checkPermission("android.permission.READ_EXTERNAL_STORAGE");
    if(QtAndroid::PermissionResult::Denied == r)
    {
        lstPermission << "android.permission.READ_EXTERNAL_STORAGE";
    }
    r = QtAndroid::checkPermission("android.permission.WRITE_EXTERNAL_STORAGE");
    if(QtAndroid::PermissionResult::Denied == r)
    {
        lstPermission << "android.permission.WRITE_EXTERNAL_STORAGE";
    }
    if(!lstPermission.isEmpty())
        QtAndroid::requestPermissionsSync(lstPermission);
#else
    
    /* Checks if the app has permission to read and write to device storage
     * If the app does not has permission then the user will be prompted to
     * grant permissions, When android > 6.0(SDK API > 23)
     */
    QAndroidJniObject mainActive = QtAndroid::androidActivity();
    CHECK_EXCEPTION();
    if(mainActive.isValid())
    {
        QAndroidJniObject::callStaticMethod<void>(
                "org/KangLinStudio/QtAndroidUtils/Utils",
                "verifyStoragePermissions",
                "(Landroid/app/Activity;)V",
                mainActive.object<jobject>());
        CHECK_EXCEPTION();
    }
    else
    {
        qDebug() << "QtAndroid::androidActivity() isn't valid\n";
    }
#endif

    return nRet;
}

int CAndroidUtils::InitCameraPermissions()
{
    int nRet = 0;
    QAndroidJniEnvironment env;
    
    /*
     The following permission must be set in AndroidManifest.xml:
     <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
     <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>
    */
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    QStringList lstPermission;
    QtAndroid::PermissionResult r;
    r = QtAndroid::checkPermission("android.permission.CAMERA");
    if(QtAndroid::PermissionResult::Denied == r)
    {
        lstPermission << "android.permission.CAMERA";
    }
    if(!lstPermission.isEmpty())
        QtAndroid::requestPermissionsSync(lstPermission);
#else
    
    /* Checks if the app has permission to read and write to device storage
     * If the app does not has permission then the user will be prompted to
     * grant permissions, When android > 6.0(SDK API > 23)
     */
    QAndroidJniObject mainActive = QtAndroid::androidActivity();
    CHECK_EXCEPTION();
    if(mainActive.isValid())
    {
        QAndroidJniObject::callStaticMethod<void>(
                "org/KangLinStudio/QtAndroidUtils/Utils",
                "verifyCameraPermissions",
                "(Landroid/app/Activity;)V",
                mainActive.object<jobject>());
        CHECK_EXCEPTION();
    }
    else
    {
        qDebug() << "QtAndroid::androidActivity() isn't valid\n";
    }
#endif
    return nRet;
}

int CAndroidUtils::CallPhone(const QString szNumber)
{
    int nRet = 0;
    QAndroidJniEnvironment env;
    QAndroidJniObject objAction = QAndroidJniObject::getStaticObjectField<jstring>(
                "android/content/Intent",
                "ACTION_DIAL");
    CHECK_EXCEPTION()
    
    QString szPhone = "tel:" + szNumber;
    QAndroidJniObject uri = QAndroidJniObject::callStaticObjectMethod(
                "android/net/Uri",
                "parse",
                "(Ljava/lang/String;)Landroid/net/Uri;",
                QAndroidJniObject::fromString(szPhone).object<jstring>());
    CHECK_EXCEPTION()
    QAndroidJniObject intent("android/content/Intent",
                             "(Ljava/lang/String;Landroid/net/Uri;)V",
                             objAction.object<jobject>(),
                             uri.object<jobject>());
    CHECK_EXCEPTION()
    QtAndroid::startActivity(intent,
                             CActivityResultReceiver::RESULT_CODE_PHONE
                             );
    CHECK_EXCEPTION()
    return nRet;
}

/*
  The following permission must be set in AndroidManifest.xml:
  <uses-permission android:name="android.permission.VIBRATE"/>
*/
int CAndroidUtils::Vibrate(long duration)
{
    QAndroidJniEnvironment env;
    QAndroidJniObject active = QtAndroid::androidActivity();
    CHECK_EXCEPTION()
    QAndroidJniObject name = QAndroidJniObject::getStaticObjectField(
                "android/content/Context",
                "VIBRATOR_SERVICE",
                "Ljava/lang/String;"
                );
    CHECK_EXCEPTION()
    QAndroidJniObject vibrateService = active.callObjectMethod(
                "getSystemService",
                "(Ljava/lang/String;)Ljava/lang/Object;",
                name.object<jstring>());
    CHECK_EXCEPTION()
    if(!vibrateService.isValid())
    {
        qDebug() << "vibrateService isn't valid";
        return -1;
    }
    jlong d = duration;
    vibrateService.callMethod<void>("vibrate", "(J)V", d);
    CHECK_EXCEPTION()
    return 0;
}

/*
  The following permission must be set in AndroidManifest.xml:
  <uses-permission android:name="android.permission.WAKE_LOCK"/>
  
  参见： https://blog.csdn.net/qq_32115439/article/details/80169222
        https://www.cnblogs.com/leipDao/p/8241468.html
        
  注意：必須成对使用（acquire、release）
*/
bool CAndroidUtils::PowerWakeLock(bool bWake)
{
    QAndroidJniEnvironment env;
    static QAndroidJniObject screenLock;
    if(!screenLock.isValid())
    {
        QAndroidJniObject activity = QtAndroid::androidActivity();
        CHECK_EXCEPTION()
        QAndroidJniObject name = QAndroidJniObject::getStaticObjectField(
                    "android/content/Context",
                    "POWER_SERVICE",
                    "Ljava/lang/String;"
                    );
        CHECK_EXCEPTION()
        QAndroidJniObject powerService = activity.callObjectMethod(
                    "getSystemService",
                    "(Ljava/lang/String;)Ljava/lang/Object;",
                    name.object<jstring>());
        CHECK_EXCEPTION()
        QAndroidJniObject tag = QAndroidJniObject::fromString("QtJniWakeLock");
        CHECK_EXCEPTION()
        jint screenBrightWakeLock = QAndroidJniObject::getStaticField<jint>(
                    "android/os/PowerManager",
                    "SCREEN_BRIGHT_WAKE_LOCK"
                    );
        CHECK_EXCEPTION()
        jint onAfterRelease = QAndroidJniObject::getStaticField<jint>(
                    "android/os/PowerManager",
                    "ON_AFTER_RELEASE"
                    );
        CHECK_EXCEPTION()
        jint flag = screenBrightWakeLock|onAfterRelease;
        screenLock = powerService.callObjectMethod(
                    "newWakeLock",
                    "(ILjava/lang/String;)Landroid/os/PowerManager$WakeLock;",
                    flag, //10, //SCREEN_BRIGHT_WAKE_LOCK
                    tag.object<jstring>()
                    );
        CHECK_EXCEPTION()
    }
    
    if(!screenLock.isValid())
        return false;
    
    if(bWake)
        screenLock.callMethod<void>("acquire");
    else
        screenLock.callMethod<void>("release");
    
    CHECK_EXCEPTION()
    return true;
}

bool CAndroidUtils::PowerSleep(bool bSleep)
{
    QAndroidJniEnvironment env;
    env->ExceptionClear();
    QAndroidJniObject activity = QtAndroid::androidActivity();
    CHECK_EXCEPTION()
    QAndroidJniObject name = QAndroidJniObject::getStaticObjectField(
                "android/content/Context",
                "POWER_SERVICE",
                "Ljava/lang/String;"
                );
    CHECK_EXCEPTION()
    QAndroidJniObject powerService = activity.callObjectMethod(
                "getSystemService",
                "(Ljava/lang/String;)Ljava/lang/Object;",
                name.object<jstring>());
    CHECK_EXCEPTION()

    jlong tm = QAndroidJniObject::callStaticMethod<jlong>(
                "android.os.SystemClock",
                "uptimeMillis"
                );
    CHECK_EXCEPTION()
    if(!powerService.isValid())
    {
        qDebug() << "POWER_SERVICE isn't valid";
        return false;
    }
    
    if(bSleep)
    {
        powerService.callMethod<void>("goToSleep", "(J)V", tm);
        CHECK_EXCEPTION()
    }else{
        powerService.callMethod<void>("wakeUp", "(J)V", tm);
        CHECK_EXCEPTION()
    }
    
    return true;
}

void CAndroidUtils::Reboot()
{
    QAndroidJniEnvironment env;
    env->ExceptionClear();
    QAndroidJniObject activity = QtAndroid::androidActivity();
    CHECK_EXCEPTION()
    QAndroidJniObject name = QAndroidJniObject::getStaticObjectField(
                "android/content/Context",
                "POWER_SERVICE",
                "Ljava/lang/String;"
                );
    CHECK_EXCEPTION()
    QAndroidJniObject powerService = activity.callObjectMethod(
                "getSystemService",
                "(Ljava/lang/String;)Ljava/lang/Object;",
                name.object<jstring>());
    CHECK_EXCEPTION()
    if(!powerService.isValid())
    {
        qDebug() << "POWER_SERVICE isn't valid";
        return;
    }
    
    QAndroidJniObject objReason = QAndroidJniObject::fromString(QString("recovery"));
    powerService.callMethod<void>("reboot", "(Ljava/lang/String;)V",
                                  objReason.object<jstring>());
}

int CAndroidUtils::InstallApk(const QString szFile)
{
    int nRet = 0;
    if(szFile.isEmpty())
        return 0;
    QAndroidJniEnvironment env;
    QAndroidJniObject mainActive = QtAndroid::androidActivity();
    CHECK_EXCEPTION()
    if(mainActive.isValid())
    {
        QAndroidJniObject objFile = QAndroidJniObject::fromString(szFile);
        CHECK_EXCEPTION()
        QAndroidJniObject::callStaticMethod<void>(
                "org/KangLinStudio/QtAndroidUtils/Utils",
                "install",
                "(Landroid/content/Context;Ljava/lang/String;)V",
                mainActive.object<jobject>(),
                objFile.object<jstring>());
        CHECK_EXCEPTION()
    }
    else
    {
        qDebug() << "QtAndroid::androidActivity() isn't valid\n";
    }
    return nRet;
}

int CAndroidUtils::UninstallApk(const QString szPackageName)
{
    int nRet = 0;
    if(szPackageName.isEmpty())
        return 0;
    QAndroidJniEnvironment env;
    QAndroidJniObject mainActive = QtAndroid::androidActivity();
    CHECK_EXCEPTION()
    if(mainActive.isValid())
    {
        QAndroidJniObject objFile = QAndroidJniObject::fromString(szPackageName);
        CHECK_EXCEPTION()
        QAndroidJniObject::callStaticMethod<void>(
                "org/KangLinStudio/QtAndroidUtils/Utils",
                "uninstall",
                "(Landroid/content/Context;Ljava/lang/String;)V",
                mainActive.object<jobject>(),
                objFile.object<jstring>());
        CHECK_EXCEPTION()
    }
    else
    {
        qDebug() << "QtAndroid::androidActivity() isn't valid\n";
    }
    return nRet;
}

QString CAndroidUtils::GetAppClassName()
{
    QAndroidJniObject appInfo = QtAndroid::androidActivity().callObjectMethod(
                    "getApplicationInfo",
                    "()Landroid/content/pm/ApplicationInfo;");
    
    return appInfo.getObjectField<jstring>("className").toString();
}

QString CAndroidUtils::GetAppPackageName()
{
    return QtAndroid::androidActivity().callObjectMethod<jstring>("getPackageName").toString();
}

void CAndroidUtils::Share(const QString &title,
                          const QString &subject,
                          const QString &content,
                          const QString &htmlContext,
                          const QStringList &imageFiles)
{
    QAndroidJniEnvironment env;
    QAndroidJniObject jTitle = QAndroidJniObject::fromString(title);
    QAndroidJniObject jSubject = QAndroidJniObject::fromString(subject);
    QAndroidJniObject jContent = QAndroidJniObject::fromString(content);
    QAndroidJniObject jHtmlContext = QAndroidJniObject::fromString(htmlContext);
    
    //See: https://blog.csdn.net/ljeagle/article/details/6697360)
    int size = imageFiles.size();
    jclass js = env->FindClass("java/lang/String");
    jobjectArray joaImgFiles = env->NewObjectArray(size, js, NULL);
    if(NULL == joaImgFiles)
        return;
    for(int i = 0; i < size; i++)
    {
        QAndroidJniObject s = QAndroidJniObject::fromString(imageFiles.at(i));
        env->SetObjectArrayElement(joaImgFiles, i, s.object<jstring>());
    }
    
    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject::callStaticMethod<void>(
        "org/KangLinStudio/QtAndroidUtils/Utils",
        "share",
        "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;)V",
        activity.object<jobject>(),
        jTitle.object<jstring>(),
        jSubject.object<jstring>(),
        jContent.object<jstring>(),
        jHtmlContext.object<jstring>(),
        joaImgFiles
   );
   CHECK_EXCEPTION()

   env->DeleteLocalRef(joaImgFiles);
}

void CAndroidUtils::OpenCamera()
{
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))   
    //https://github.com/DmcSDK/MediaPickerPoject
    QAndroidIntent intent(activity, "com.dmcbig.mediapicker.TakePhotoActivity");
    CHECK_EXCEPTION()
    QtAndroid::startActivity(intent.handle(),
                             CActivityResultReceiver::RESULT_CODE_CAMERA,
                             m_pResultReceiver);
    CHECK_EXCEPTION()
#else
    
    jclass clsTakePhotoActivity =
            env->FindClass("com/dmcbig/mediapicker/TakePhotoActivity");
    QAndroidJniObject intent("android/content/Intent",
                             "(Landroid/content/Context;Ljava/lang/Class;)V",
                             activity.object<jobject>(),
                             clsTakePhotoActivity);
    CHECK_EXCEPTION()
    QtAndroid::startActivity(intent,
                             CActivityResultReceiver::RESULT_CODE_CAMERA,
                             m_pResultReceiver);
    CHECK_EXCEPTION()
#endif   
}

void CAndroidUtils::OpenAlbum(int maxSelect)
{
    QAndroidJniEnvironment env;
    QAndroidJniObject activity = QtAndroid::androidActivity();
    QAndroidJniObject maxSelectCount = 
            QAndroidJniObject::getStaticObjectField<jstring>(
                "com.dmcbig.mediapicker.PickerConfig",
                "MAX_SELECT_COUNT");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))   
    //https://github.com/DmcSDK/MediaPickerPoject
    QAndroidIntent intent(activity, "com.dmcbig.mediapicker.PickerActivity");
    //intent.putExtra(maxSelectCount.toString(), QVariant(maxSelect));
    intent.handle().callObjectMethod("putExtra",
                            "(Ljava/lang/String;I)Landroid/content/Intent;",
                            maxSelectCount.object<jstring>(),
                            maxSelect);
    CHECK_EXCEPTION()
    QtAndroid::startActivity(intent.handle(),
                             CActivityResultReceiver::RESULT_CODE_PHOTO,
                             m_pResultReceiver);
    CHECK_EXCEPTION()
#else
    
    jclass clsPickerActivity = env->FindClass("com/dmcbig/mediapicker/PickerActivity");
    QAndroidJniObject intent("android/content/Intent",
                             "(Landroid/content/Context;Ljava/lang/Class;)V",
                             activity.object<jobject>(),
                             clsPickerActivity);
    CHECK_EXCEPTION()//*/
    /*
    QAndroidJniObject objPA = 
        QAndroidJniObject::fromString("com.dmcbig.mediapicker.PickerActivity");
    QAndroidJniObject objPickerActivity = 
        QAndroidJniObject::callStaticObjectMethod(
                "java/lang/Class",
                "forName",
                "(Ljava/lang/String;)Ljava/lang/Class;",
                objPA.object<jstring>());
    CHECK_EXCEPTION()
    QAndroidJniObject intent("android.content.Intent",
                             "(Landroid/content/Context;Ljava/lang/Class;)V",
                             activity.object<jobject>(),
                             objPickerActivity.object<jclass>());
    CHECK_EXCEPTION()//*/
    intent.callObjectMethod("putExtra",
                            "(Ljava/lang/String;I)Landroid/content/Intent;",
                            maxSelectCount.object<jstring>(),
                            maxSelect);
    QtAndroid::startActivity(intent,
                             CActivityResultReceiver::RESULT_CODE_PHOTO,
                             m_pResultReceiver);
    CHECK_EXCEPTION()
#endif   
}

void CAndroidUtils::SelectPhotos(QStringList path)
{
    emit sigSelectPhotos(path);
}
