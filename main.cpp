#include "./src/MainTableWidget/MainTableWidget.h"
#include "./src/ToolBox/ToolBox.h"
#include"./src/DayTimerWidget/DayTimerWidget.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include<QDir>
#include<QDateTime>
#include<QProcess>
#include<QFontDatabase>
#include<QStyleFactory>
#include <iostream>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)    
# pragma execution_character_set("utf-8")    
#endif
#include"./src/AppLog/AppLog.h"
#include"./src/NetworkRequests/NetworkRequests.h"
#include"./src/SyncingDialog/SyncingDialog.h"
#include "./src/Utils/Utils.h"
bool ToolBoxisOpen(){
    QFile file(QDir::currentPath() + "/config.json");
    file.open(QIODevice::ReadWrite | QIODevice::Text);

    QTextStream stream(&file);
    QString file_str = stream.readAll();
    file.close();
    QJsonParseError jsonError;
    QJsonDocument jsondoc = QJsonDocument::fromJson(file_str.toUtf8(),&jsonError);
    if (jsonError.error != QJsonParseError::NoError && !jsondoc.isNull()) {
        showLog("Config.json is Error!",LogStatus::ERR);
        return true;
    }
    QJsonObject Config = jsondoc.object();
    if (Config.contains("toolbox_status")){
        if (Config["toolbox_status"].toBool()){
            showLog("ToolBox is Not Show",LogStatus::INFO);
            return false;
        }else{
            showLog("ToolBox is Show",LogStatus::INFO);
            return true;
        }

    }
    showLog("ToolBox is Show",LogStatus::INFO);
    return true;
}
bool TimerisOpen(){
    QFile file(QDir::currentPath() + "/config.json");
    file.open(QIODevice::ReadWrite | QIODevice::Text);

    QTextStream stream(&file);
    QString file_str = stream.readAll();
    file.close();
    QJsonParseError jsonError;
    QJsonDocument jsondoc = QJsonDocument::fromJson(file_str.toUtf8(),&jsonError);
    if (jsonError.error != QJsonParseError::NoError && !jsondoc.isNull()) {
        showLog("Config.json is Error!",LogStatus::ERR);
        return true;
    }
    QJsonObject Config = jsondoc.object();
    if (Config.contains("disable_timer")){

        if (Config["disable_timer"].toBool()){
            showLog("Timer is Not Show",LogStatus::INFO);
            return false;
        }else{
            showLog("Timer is Show",LogStatus::INFO);
            return true;
        }
    }
    showLog("Timer is Show",LogStatus::INFO);
    return true;
}
bool SyncisOpen(){
    QFile file(QDir::currentPath() + "/config.json");
    file.open(QIODevice::ReadWrite | QIODevice::Text);

    QTextStream stream(&file);
    QString file_str = stream.readAll();
    file.close();
    QJsonParseError jsonError;
    QJsonDocument jsondoc = QJsonDocument::fromJson(file_str.toUtf8(),&jsonError);
    if (jsonError.error != QJsonParseError::NoError && !jsondoc.isNull()) {
        showLog("Config.json is Error!",LogStatus::ERR);
        return true;
    }
    QJsonObject Config = jsondoc.object();
    if (Config.contains("sync_open")){
        if (!Config["sync_open"].toBool()){
            showLog("Sync is Not Open",LogStatus::INFO);
            return false;
        }else{
            showLog("Sync is open",LogStatus::INFO);
            return true;
        }
    }
    showLog("Sync is not open",LogStatus::INFO);
    return false;
}
void printLogo() {
    QFile file(":/res/logo.txt");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QTextStream stream(&file);
    QString file_str = stream.readAll();
    file.close();
    std::cout << file_str.toStdString() << std::endl;
    std::cout << "---------------------------------------------------------------------------" << std::endl;
}
int main(int argc, char *argv[])
{
    // 插入 -platform xcb 参数
    char *new_argv[argc + 2];
    new_argv[0] = argv[0];
    new_argv[1] = const_cast<char*>("-platform");
    new_argv[2] = const_cast<char*>("xcb");

    for (int i = 1; i < argc; ++i) {
        new_argv[i + 2] = argv[i];
    }

    int new_argc = argc + 2;
    QApplication a(new_argc, new_argv);
    //QApplication::setAttribute(Qt::AA_SetPlatformPlugin, QVariant("xcb"));
    printLogo();

    a.setApplicationDisplayName("ClassTopLand");
    a.setStyleSheet(getStyleSheet(":/qss/global.qss"));
    QApplication::setQuitOnLastWindowClosed(false);
    showLog("MainWindow is Show",LogStatus::INFO);
    SyncingDialog *syncDialog = new SyncingDialog();

    if (SyncisOpen())
    {
        syncDialog->syncRemoteJson();
    }
    MainTableWidget *w = new MainTableWidget();
    w->show();

    QScreen *screen = QApplication::primaryScreen();
    // 逻辑尺寸
    QSize logicalSize = screen->size();  // 或 screen->availableSize()

    // 物理尺寸（实际像素）
    qreal ratio = screen->devicePixelRatio();
    QSize physicalSize = logicalSize * ratio;
    int scr_w = physicalSize.width();
    int scr_h = physicalSize.height();
    qDebug() << "scr_w:" << scr_w;
    qDebug() << "scr_h:" << scr_h;
    w->move((scr_w-w->width())/2, 0);
    ToolBox *tb = new ToolBox();
    qDebug() << w->pos();

    qDebug() << tb->pos();
    if (ToolBoxisOpen()){
        tb->show();
    }
    tb->move(scr_w - tb->width(), (scr_h - tb->height()) / 2 * 0.6);
    DayTimerWidget *dtw = new DayTimerWidget();
    dtw->move((scr_w - dtw->width()),(scr_h - dtw->height()) * 0.95);
    if (TimerisOpen()) {
        dtw->show();
    }
    QObject::connect(w->EditWindow,&TableEditWidget::refechToolBar_signal,tb,&ToolBox::LoadPlugins);
    return a.exec();
}
