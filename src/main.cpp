/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *               2011 ~ 2018 Wang Yong
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Wang Yong <wangyong@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "main_window.h"
#include "screenshot.h"
#include "utils.h"
#include "widgets/toolbutton.h"
#include "dbusservice/dbusscreenshotservice.h"
#include "accessibility/acObjectList.h"

#include <DWidget>
#include <DLog>
#include <DWindowManagerHelper>
#include <DWidgetUtil>
#include <DGuiApplicationHelper>
#include <DApplication>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QDir>

DWIDGET_USE_NAMESPACE

bool isWaylandProtocol()
{
    QProcessEnvironment e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));
    return XDG_SESSION_TYPE == QLatin1String("wayland") ||  WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive);
}


int main(int argc, char *argv[])
{

    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")){
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1);
    }
    DGuiApplicationHelper::setUseInactiveColorGroup(false);

    // 平板模式
    Utils::isTabletEnvironment = false; // DGuiApplicationHelper::isTabletEnvironment();
    qDebug() << "Is Table:" << Utils::isTabletEnvironment;

    // wayland 协议
    Utils::isWaylandMode = isWaylandProtocol();
    qDebug() << "Is Wayland:" << Utils::isWaylandMode;

    Utils::isRootUser = (getuid() == 0);
    qDebug() << "Is Root User:" << Utils::isRootUser;

    if(Utils::isWaylandMode) {
       qputenv("QT_WAYLAND_SHELL_INTEGRATION", "kwayland-shell");
    }

#ifdef KF5_WAYLAND_FLAGE_ON
    qDebug() << "KF5_WAYLAND_FLAGE_ON is open!!";
#else
    qDebug() << "KF5_WAYLAND_FLAGE_ON is close!!";
#endif
    // 适配deepin-turbo 启动加速
#if(DTK_VERSION < DTK_VERSION_CHECK(5,4,0,0))
    DApplication::loadDXcbPlugin();
    QScopedPointer<DApplication> app(new DApplication(argc, argv));
#else
    QScopedPointer<DApplication> app(DApplication::globalApplication(argc,argv));
#endif



    // 集成测试标签
#ifdef ENABLE_ACCESSIBILITY
    QAccessible::installFactory(accessibleFactory);
#endif


    QDBusConnection dbus = QDBusConnection::sessionBus();
    //    dbus.registerService("com.deepin.ScreenRecorder");
    if (dbus.registerService("com.deepin.ScreenRecorder")) {
        app->setOrganizationName("deepin");
        app->setApplicationName("deepin-screen-recorder");
        app->setApplicationVersion("1.0");
        app->setAttribute(Qt::AA_UseHighDpiPixmaps);

        static const QDate buildDate = QLocale( QLocale::English ).
                toDate( QString(__DATE__).replace("  ", " 0"), "MMM dd yyyy");
        QString t_date = buildDate.toString("MMdd");

        // Version Time
        app->setApplicationVersion(DApplication::buildVersion(t_date));

        using namespace Dtk::Core;
        Dtk::Core::DLogManager::registerConsoleAppender();
        Dtk::Core::DLogManager::registerFileAppender();

        QCommandLineOption  delayOption(QStringList() << "d" << "delay", "Take a screenshot after NUM seconds.", "NUM");
        QCommandLineOption fullscreenOption(QStringList() << "f" << "fullscreen", "Take a screenshot the whole screen.");
        QCommandLineOption topWindowOption(QStringList() << "w" << "top-window", "Take a screenshot of the most top window.");
        QCommandLineOption savePathOption(QStringList() << "s" << "save-path","Specify a path to save the screenshot.", "PATH");
        QCommandLineOption prohibitNotifyOption(QStringList() << "n" << "no-notification","Don't send notifications.");
        QCommandLineOption dbusOption(QStringList() << "u" << "dbus", "Start  from dbus.");
        QCommandLineOption screenRecordOption(QStringList() << "record" << "screenRecord" << "start screen record");
        QCommandLineOption screenShotOption(QStringList() << "shot" << "screenShot" << "start screen shot");
        QCommandLineOption screenOcrOption(QStringList() << "ocr" << "screenOcr" << "start screen ocr");
        QCommandLineOption screenScrollOption(QStringList() << "scroll" << "screenScroll" << "start screen scroll");


        QCommandLineParser cmdParser;
        cmdParser.setApplicationDescription("deepin-screen-recorder");
        cmdParser.addHelpOption();
        cmdParser.addVersionOption();
        cmdParser.addOption(delayOption);
        cmdParser.addOption(fullscreenOption);
        cmdParser.addOption(topWindowOption);
        cmdParser.addOption(savePathOption);
        cmdParser.addOption(prohibitNotifyOption);
        cmdParser.addOption(dbusOption);
        cmdParser.addOption(screenRecordOption);
        cmdParser.addOption(screenShotOption);
        cmdParser.addOption(screenOcrOption);
        cmdParser.addOption(screenScrollOption);
        cmdParser.process(*app);

        // Load translator.
        app->loadTranslator();
        Screenshot window;

        // 主题设置
        DGuiApplicationHelper::ColorType t_type = DGuiApplicationHelper::instance()->themeType();
        DGuiApplicationHelper::instance()->setPaletteType(t_type);
        Utils::themeType = t_type;


        QString t_launchMode = "screenShot";
        if(cmdParser.isSet(screenRecordOption)) {
            t_launchMode = "screenRecord";
        } else if (cmdParser.isSet(screenOcrOption)) {
            t_launchMode = "screenOcr";
        } else if (cmdParser.isSet(screenScrollOption)) {
            t_launchMode = "screenScroll";
        }
        window.initLaunchMode(t_launchMode);
        DBusScreenshotService dbusService (&window);
        if (Utils::isWaylandMode) {
            //Wayland下不支持多屏状态下的录屏操作
            if (QApplication::desktop()->screenCount() != 1) {
                if (t_launchMode == "screenRecord") {
                    Utils::notSupportWarn();
                    _Exit(0);
                    qApp->quit();
                    return 0;
                }
            }
        }
        // Register debus service.
        dbus.registerObject("/com/deepin/ScreenRecorder", &window, QDBusConnection::ExportScriptableSignals | QDBusConnection::ExportScriptableSlots);
        QDBusConnection conn = QDBusConnection::sessionBus();

        if (!conn.registerService("com.deepin.Screenshot") || !conn.registerObject("/com/deepin/Screenshot", &window)) {
            qDebug() << "deepin-screenshot is running!";
            qApp->quit();
            return 0;
        }


        if (cmdParser.isSet(dbusOption)) {
            qDebug() << "dbus register waiting!";
            return app->exec();

        } else {
            dbusService.setSingleInstance(true);
            if (cmdParser.isSet(delayOption)) {
                qDebug() << "cmd delay screenshot";
                window.delayScreenshot(cmdParser.value(delayOption).toInt());
            } else if (cmdParser.isSet(fullscreenOption)) {
                window.fullscreenScreenshot();
            } else if (cmdParser.isSet(topWindowOption)) {
                qDebug() << "cmd topWindow screenshot";
                window.topWindowScreenshot();
            } else if (cmdParser.isSet(savePathOption)) {
                qDebug() << "cmd savepath screenshot";
                window.savePathScreenshot(cmdParser.value(savePathOption));
            } else if (cmdParser.isSet(prohibitNotifyOption)) {
                qDebug() << "screenshot no notify!";
                window.noNotifyScreenshot();
            } else {
                window.startScreenshot();
            }
        }

        return app->exec();
    } else {
        //      Send DBus message to stop screen - recorder if found other screen - recorder DBus service has started.
        QDBusInterface notification("com.deepin.ScreenRecorder",
                                    "/com/deepin/ScreenRecorder",
                                    "com.deepin.ScreenRecorder",
                                    QDBusConnection::sessionBus());

        QList<QVariant> arg;
        notification.callWithArgumentList(QDBus::AutoDetect, "stopRecord", arg);
    }

    return 0;
}
