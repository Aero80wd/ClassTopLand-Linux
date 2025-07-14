#include "./MainTableWidget.h"
#include "ui_MainTableWidget.h"

#include "../API.h"

MainTableWidget::MainTableWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainTableWidget)
{
    ui->setupUi(this);
    EditWindow = new TableEditWidget();
    readConfig();
    readTimeTable();
    
    EditWindow->setConfig(Config);

    initUi();
    rtt = new refechTableThread();

    initSignal();

    rtt->start();
    initSysTrayIcon();
    QTranslator translator;
    QLocale::Language lab = QLocale::system().language();
    if(QLocale::Chinese == lab)
    {
        translator.load(":/lang/lang_cn.qm");
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
    }else if(QLocale::English== lab){
        translator.load(":/language/lang_en.qm");
        qApp->installTranslator(&translator);
        ui->retranslateUi(this);
    }

}

MainTableWidget::~MainTableWidget()
{
    delete ui;
}
//注意：需要包含头文件#include <QProcess>
bool refechTableThread::WhetherProcessRunning(QString& processName)
{
    // HWND swid = FindWindow(NULL, processName.toStdWString().c_str());
    // return swid != NULL;
    return false;
}
void MainTableWidget::on_startTimer(QString timer_str)
{
    timerStart = true;
    QStringList strList = timer_str.split(":");
    min_time = strList[0].toLongLong();
    sec_time = strList[1].toLongLong();
    if (min_time == 0 && sec_time == 0)
    {
        timerStart=false;
        showStatus("倒计时结束");
        return;
    }
    timer_id = startTimer(1000);

}
void MainTableWidget::on_stopTimer()
{
    timerStart = false;
    killTimer(timer_id);
}
void MainTableWidget::on_timerisStart(bool &st)
{
    st = timerStart;
}
void MainTableWidget::timerEvent(QTimerEvent *event)
{

    if (sec_time == 0 && min_time >0)
    {
        sec_time = 59;
        min_time--;
    }else
    {
        sec_time--;
    }
    if (min_time == 0 && sec_time == 0)
    {
        timerStart=false;
        killTimer(timer_id);
        showStatus("倒计时结束");
        emit reText();
        return;
    }
    ui->label_4->setText(QString("%1:%2").arg(min_time, 2, 10, QLatin1Char('0')).arg(sec_time, 2, 10, QLatin1Char('0')));
    if (sec_time % 15 ==0 || (min_time == 0 && sec_time == 5))
    {
        emit showTimer();

    }
}
void MainTableWidget::on_showTimer()
{
    timer_animation->setDirection(QAbstractAnimation::Forward);
    timer_animation->start();
    QEventLoop loop;//定义一个新的事件循环
    QTimer::singleShot(5000, &loop, SLOT(quit()));//创建单次定时器，槽函数为事件循环的退出函数
    loop.exec();//事件循环开始执行，程序会卡在这里，直到定时时间到，本循环被退出
    timer_animation->setDirection(QAbstractAnimation::Backward);
    timer_animation->start();
}
void MainTableWidget::showStatus(QString str)
{
    showLog("ShowStatused!",INFO);
    ui->label_3->setText(str);
    status_msg_animation->setDirection(QAbstractAnimation::Forward);
    status_msg_animation->start();
    QTimer::singleShot(5000,this,[&]()
    {
        status_msg_animation->setDirection(QAbstractAnimation::Backward);
        status_msg_animation->start();
    });
}
void MainTableWidget::on_getTimer(int &m,int &s)
{
    m = min_time;
    s = sec_time;
}

void MainTableWidget::initAnimation()
{
    QScreen *scr = qApp->primaryScreen();
    int scr_w = scr->size().width();
    int scr_h = scr->size().height();
    move((scr_w - width()) / 2, 0);
    move((scr_w - width()) / 2, 0);
    ui->timer_show->move(width() + ui->timer_show->width(),0);
    // 窗口隐藏/显示动画
    hide_animation = new QPropertyAnimation(this,"pos");
    hide_animation->setDuration(500);
    hide_animation->setEasingCurve(QEasingCurve::InOutSine);
    hide_animation->setStartValue(pos());
    hide_animation->setEndValue(QPoint(scr_w-21,0));
    // 计时器隐藏/显示动画
    timer_animation = new QPropertyAnimation(ui->timer_show,"pos");
    timer_animation->setDuration(1000);
    timer_animation->setEasingCurve(QEasingCurve::InOutSine);
    timer_animation->setStartValue(ui->timer_show->pos());
    timer_animation->setEndValue(QPoint(width()-ui->timer_show->width(),0));
}
void MainTableWidget::initUi(){
    setWindowFlags(Qt::WindowType::FramelessWindowHint | Qt::WindowType::WindowStaysOnTopHint);
    setAttribute(Qt::WidgetAttribute::WA_TranslucentBackground);
    

    GlassHelper::enableBlurBehind(this);
    status_msg_animation = new QPropertyAnimation(ui->status_show,"pos");
    status_msg_animation->setDuration(500);
    status_msg_animation->setEasingCurve(QEasingCurve::InOutSine);
    status_msg_animation->setStartValue(QPoint(0,-49));
    status_msg_animation->setEndValue(QPoint(0,0));
    class_show_widget_layout = new QHBoxLayout();
    class_show_widget_layout->setContentsMargins(0,0,0,0);
    class_show_widget_layout->setSpacing(0);
    ui->class_show_widget->setLayout(class_show_widget_layout);


//    setWidgetBlur(ui->table_show_2);
//    setWidgetBlur(ui->table_show_3);
//    setWidgetBlur(ui->pushButton_2);
//    setWidgetBlur(ui->pushButton_3);
//    setWidgetBlur(ui->pushButton_4);
//    setWidgetBlur(ui->pushButton_5);


}
void MainTableWidget::on_hideWindow()
{
    if (!WindowHide)
    {
        hide_animation->setDirection(QAbstractAnimation::Forward);
        hide_animation->start();
        WindowHide = true;
    }else
    {
        hide_animation->setDirection(QAbstractAnimation::Backward);
        hide_animation->start();
        WindowHide = false;
    }
}
void MainTableWidget::initSignal(){

    connect(EditWindow,SIGNAL(refechTable_signal()),this,SLOT(refechTable_slot()));
    connect(rtt,&refechTableThread::repaint,this,&MainTableWidget::do_repaint,Qt::QueuedConnection);
    connect(rtt,&refechTableThread::tst,ui->class_time,&QLabel::setText,Qt::QueuedConnection);
    connect(rtt,&refechTableThread::showStatusMessage,this,&MainTableWidget::showStatus,Qt::QueuedConnection);
    connect(rtt,&refechTableThread::changeStackedIndex,this,[=](int idx)
    {
        ui->stackedWidget->setCurrentIndex(idx);
    },Qt::QueuedConnection);
    connect(rtt,&refechTableThread::addClass,this,[=](QString text)
    {
        QLabel *class_label = new QLabel(ui->class_show_widget);
        class_show_widget_layout->addWidget(class_label);
        class_label->setText(text);
        class_label->setFixedSize(49,49);
        QFont font("Microsoft YaHei UI",16);
        class_label->setFont(font);
        class_label->setAlignment(Qt::AlignCenter);

    },Qt::QueuedConnection);
    connect(rtt,&refechTableThread::setClassStyleSheet,this,[=](int idx,QString styleSheet)
    {
        QList<QLabel*> class_list = ui->class_show_widget->findChildren<QLabel*>();
        class_list[idx]->setStyleSheet(styleSheet);
    },Qt::QueuedConnection);
    connect(rtt,&refechTableThread::toDone,this,[=]
    {
        resize(531,height());
        ui->stackedWidget->resize(531,height());
        ui->status_show->resize(531,height());
        QScreen *scr = qApp->primaryScreen();
        int scr_w = scr->size().width();
        int scr_h = scr->size().height();
        move((scr_w - width()) / 2, 0);

    },Qt::QueuedConnection);
    connect(rtt,&refechTableThread::initMainWindowAnimation,this,&MainTableWidget::initAnimation,Qt::QueuedConnection);
    connect(ui->hide_window,&QPushButton::clicked,this,&MainTableWidget::on_hideWindow);
    connect(this,&MainTableWidget::showTimer,this,&MainTableWidget::on_showTimer,Qt::QueuedConnection);
    connect(rtt, &refechTableThread::windowTop, this, [=] {
        // SetWindowPos(HWND(this->winId()), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            qDebug() << "";
    }, Qt::QueuedConnection);
    connect(rtt, &refechTableThread::getWidth, this,&MainTableWidget::on_getWidth, Qt::BlockingQueuedConnection);

}
void MainTableWidget::readTimeTable(){
    QFileInfo fi(TABLE_JSON);
    if (!fi.isFile()){
        QFile file(TABLE_JSON);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QJsonObject tempjson;
        QJsonArray nulljsonarray;
        tempjson.insert("Fri",nulljsonarray);
        tempjson.insert("Thu",nulljsonarray);
        tempjson.insert("Wed",nulljsonarray);
        tempjson.insert("Tue",nulljsonarray);
        tempjson.insert("Mon",nulljsonarray);
        QJsonDocument tempdoc;
        tempdoc.setObject(tempjson);
        file.close();
        QFile file_2(TABLE_JSON);
        file_2.open(QIODevice::ReadWrite | QIODevice::Text);
        file_2.write(tempdoc.toJson());
        file_2.close();
        QMessageBox::information(this,tr("提示"),tr("您未创建课程文件，系统已帮您创建，请手动编辑。"));
    }else{
        QFile file(TABLE_JSON);
        file.open(QIODevice::ReadWrite | QIODevice::Text);


        QTextStream stream(&file);
        QString file_str = stream.readAll();
        file.close();
        QJsonParseError jsonError;
        QJsonDocument jsondoc = QJsonDocument::fromJson(file_str.toUtf8(),&jsonError);
        if (jsonError.error != QJsonParseError::NoError && !jsondoc.isNull()) {
            showLog("Config.json is Error!",LogStatus::ERR);
            return;
        }
        time_table = jsondoc.object();
        initTodayTable();

    }
}
void MainTableWidget::readConfig(){
    if (!QFileInfo::exists(CONFIG_JSON)){
        QFile file(CONFIG_JSON);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        QJsonObject config_json;
        config_json.insert("end_time","2000-01-01 00:00:00");
        config_json.insert("english","There are () $\nleft until install");
        config_json.insert("english_end","There is not a $\nleft until install");
        config_json.insert("english_tag","install");
        config_json.insert("label_tag","安装完成");
        QJsonDocument write_doc;
        write_doc.setObject(config_json);
        file.write(write_doc.toJson());
        file.close();
        startGetStart();
    }else{
        QFile file(CONFIG_JSON);
        file.open(QIODevice::ReadWrite | QIODevice::Text);

        QTextStream stream(&file);
        QString file_str = stream.readAll();
        file.close();
        QJsonParseError jsonError;
        QJsonDocument jsondoc = QJsonDocument::fromJson(file_str.toUtf8(),&jsonError);
        if (jsonError.error != QJsonParseError::NoError && !jsondoc.isNull()) {
            showLog("Config.json is Error!",LogStatus::ERR);
            return;
        }
        Config = jsondoc.object();
    }
}
void MainTableWidget::initTodayTable(){
    QDateTime current_date_time = QDateTime::currentDateTime();
    today_table = time_table.value(current_date_time.toString("ddd")).toArray();
    resize(154+20+today_table.count()*49,height());
    ui->stackedWidget->resize(154+20+today_table.count()*49,height());
    ui->status_show->resize(154+20+today_table.count()*49,height());
}
void MainTableWidget::do_repaint(){
    return;
}
void refechTableThread::run(){
    QString display_string = "";
    QString showstr;
    QJsonObject temp;
    bool shangkelema = false;
    QDateTime current_date_time = QDateTime::currentDateTime();
    if (today_table.count() <= 0)
    {
        emit changeStackedIndex(0);
    }
    qDebug()<<today_table;
    for (int i=0;i<today_table.count();i++)
    {
        emit addClass(QString(today_table[i].toObject()["name"].toString().at(0)));
    }
    emit changeStackedIndex(1);

    emit initMainWindowAnimation();
    QJsonObject null_class = { {"start","00:00"},{"end","00:00"} };
    for(int idx = 0;idx < today_table.count();)
    {

        QStringList topprocess = { "希沃白板"};
        for (QString process : topprocess) {
            if (WhetherProcessRunning(process)) {
                emit windowTop();
            }
        }
        
        
        QDateTime current_date_time = QDateTime::currentDateTime();
        QJsonObject current_class = today_table[idx].toObject();
        QJsonObject prev_class = idx > 0 ? today_table[idx - 1].toObject() : null_class;
        QJsonObject next_class = idx < today_table.count() - 1 ? today_table[idx + 1].toObject() : null_class;
        QDateTime current_class_start_time = getTodayTime(current_class.value("start").toString());
        QDateTime current_class_end_time = getTodayTime(current_class.value("end").toString());
        QDateTime prev_class_start_time = getTodayTime(prev_class.value("start").toString());
        QDateTime prev_class_end_time = getTodayTime(prev_class.value("end").toString());
        QDateTime next_class_start_time = getTodayTime(next_class.value("start").toString());
        QDateTime next_class_end_time = getTodayTime(next_class.value("end").toString());
        if (current_date_time.secsTo(current_class_end_time)> 0) { // 当前时间 - 当前课程下课时间 >= 0 (上课中)
            if (idx > 0) emit setClassStyleSheet(idx - 1, ""); //去除上一节课的边框
            emit setClassStyleSheet(idx, "border-width: 0px 0px 4px 0px; border-color:#1191d3; border-style: solid;");
            if (current_date_time.secsTo(current_class_start_time) == 0 and !shangkelema) {  // 当前时间 - 当前课程开始时间 == 0 (刚开始上课)
                shangkelema = true;
                if (canShow(QString("%1 已经上课，请做好上课准备").arg(current_class["name"].toString()))) {
                    emit showStatusMessage(QString("%1 已经上课，请做好上课准备").arg(current_class["name"].toString()));
                }
                else if (canShow(QString("%1 已上课").arg(current_class["name"].toString()))) {
                    emit showStatusMessage(QString("%1 已上课").arg(current_class["name"].toString()));
                }
                else {
                    emit showStatusMessage(QString("上课时间到"));
                }
                
            }
            else {
                int diff_time = current_date_time.secsTo(current_class_end_time);
                int hour = diff_time / 3600;
                diff_time = diff_time % 3600;
                int min = diff_time / 60;
                int sec = diff_time % 60;
                QString display_string = QString("%1:%2:%3").arg(hour, 2, 10, QLatin1Char('0')).arg(min, 2, 10, QLatin1Char('0')).arg(sec, 2, 10, QLatin1Char('0'));
                emit tst(display_string);
                sleep(50);
            }
        }
        else if (current_date_time.secsTo(current_class_end_time) <= 0) { // 当前时间 - 当前课程下课时间 <= 0 (下课 or 不是这节课)
            if (current_date_time.secsTo(next_class_start_time)> 0) { // 当前时间 - 下一节课开始时间 >= 0 (就是这节课，且正在下课时间)
                emit setClassStyleSheet(idx, ""); //去除上一节课的边框
                emit setClassStyleSheet(idx + 1, "border-width: 0px 0px 4px 0px; border-color:rgb(0,226,142); border-style: solid;");
                if (current_date_time.secsTo(current_class_end_time) == 0 and shangkelema) {
                    shangkelema = false;
                    
                    if (canShow(QString("%1 已经下课，请做好下节课上课准备").arg(current_class["name"].toString()))) {
                        emit showStatusMessage(QString("%1 已经下课，请做好下节课上课准备").arg(current_class["name"].toString()));
                    }
                    else if (canShow(QString("%1 已下课").arg(current_class["name"].toString()))) {
                        emit showStatusMessage(QString("%1 已下课").arg(current_class["name"].toString()));
                    }
                    else {
                        emit showStatusMessage(QString("下课时间到"));
                    }
                }
                int diff_time = current_date_time.secsTo(next_class_start_time); // 当前时间 - 下一节课开始时间 (课间还剩多久)
                int hour = diff_time / 3600;
                diff_time = diff_time % 3600;
                int min = diff_time / 60;
                int sec = diff_time % 60;
                QString display_string = QString("%1:%2:%3").arg(hour, 2, 10, QLatin1Char('0')).arg(min, 2, 10, QLatin1Char('0')).arg(sec, 2, 10, QLatin1Char('0'));
                emit tst(display_string);
                if (hour == 0 && min == 2 && sec == 0) {
                    if (canShow(QString("%1 即将上课，请做好上课准备").arg(next_class["name"].toString()))) {
                        emit showStatusMessage(QString("%1 即将上课，请做好上课准备").arg(next_class["name"].toString()));
                    }
                    else if (canShow(QString("%1 即将上课").arg(next_class["name"].toString()))) {
                        emit showStatusMessage(QString("%1 即将上课").arg(next_class["name"].toString()));
                    }
                    else {
                        emit showStatusMessage(QString("即将上课"));
                    }
                }
                sleep(50);
                continue;
            }
            idx++; //下一节课
            continue;
        }
        else {
            idx++; //下一节课
            continue;
        }
        
    }
    emit changeStackedIndex(0);
    emit toDone();
    emit initMainWindowAnimation();

}
QDateTime refechTableThread::getTodayTime(QString str){
    QString timeString = str;
    QStringList timeList = timeString.split(":");
    int hour = timeList[0].toInt();
    int minute = timeList[1].toInt();

    QDateTime dateTime;
    dateTime.setDate(QDate::currentDate());
    dateTime.setTime(QTime(hour, minute));
    return dateTime;
}

void MainTableWidget::swithToYiYan(){
    yiyanDialog *yiyan = new yiyanDialog(this);
    yiyan->setConfig(Config);
    yiyan->setZuanYanOpen(ZuanYanisOpen);
    QScreen *scr = qApp->primaryScreen();
    int scr_w = scr->size().width();
    int scr_h = scr->size().height();
    yiyan->move((scr_w - yiyan->width()) / 2, (scr_h - yiyan->height()) / 3);
    yiyan->setModal(false);
    yiyan->show();
}




QString MainTableWidget::getToken(){
    QNetworkAccessManager *smsManager = new QNetworkAccessManager(this);
    QString url = "https://v2.jinrishici.com/token";
    QNetworkRequest *Request = new QNetworkRequest(QUrl(url));
    QByteArray responseData;
    QEventLoop eventLoop;
    connect(smsManager , SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
    QNetworkReply* initReply = smsManager ->get(*Request );
    eventLoop.exec();       //block until finish
    responseData = initReply->readAll();

    //解析json
    QString imageData;//接收到服务器的base64数据是string类型
    QString txt;
    QJsonParseError json_error;
    QJsonDocument doucment = QJsonDocument::fromJson(responseData, &json_error);
    if (json_error.error == QJsonParseError::NoError) {
        if (doucment.isObject()) {
            const QJsonObject obj = doucment.object();
            return obj.value("data").toString();
        }
    }
}
void MainTableWidget::on_label_clicked()
{
    swithToYiYan();
}
void MainTableWidget::hk_slot(QString day){
    rtt->terminate();
    if (day.contains("APPEND_",Qt::CaseSensitive)){
        today_table = time_table["appendixTables"].toObject()[day.split("_").last()].toArray();
    }else{
        today_table = time_table.value(day).toArray();
    }
    for (QWidget*widget : ui->class_show_widget->findChildren<QWidget*>())
    {
        delete widget;
    }
    rtt->start();
    resize(154+20+today_table.count()*49,height());
    ui->stackedWidget->resize(154+20+today_table.count()*49,height());
    ui->status_show->resize(154+20+today_table.count()*49,height());
    QScreen *scr = qApp->primaryScreen();
    int scr_w = scr->size().width();
    int scr_h = scr->size().height();
    move((scr_w - width()) / 2, 0);
}



void MainTableWidget::initSysTrayIcon()
{


    //新建QSystemTrayIcon对象
    m_sysTrayIcon = new QSystemTrayIcon(this);

    //设置托盘图标
    QIcon icon = QIcon(":/res/icon.png");    //资源文件添加的图标
    m_sysTrayIcon->setIcon(icon);

    //当鼠标移动到托盘上的图标时，会显示此处设置的内容
    m_sysTrayIcon->setToolTip("ClassTopLand");

    // //给QSystemTrayIcon添加槽函数
    connect(m_sysTrayIcon, &QSystemTrayIcon::activated,
            [=](QSystemTrayIcon::ActivationReason reason)
            {
                switch(reason)
                {
                case QSystemTrayIcon::Trigger:
                    m_showmain->activate(QAction::Trigger);
                    break;
                default:
                    break;
                }
            });

    //建立托盘操作的菜单
    createActions();
    createMenu();


    //在系统托盘显示此对象
    m_sysTrayIcon->show();
}
void MainTableWidget::createActions(){
    m_showedit = new QAction(tr("设置"),this);
    connect(m_showedit,SIGNAL(triggered()),this,SLOT(on_showMainAction()));
    m_showmain = new QAction(tr("打开主界面"),this);
    connect(m_showmain,&QAction::triggered,this,&MainTableWidget::startMainWindow);
    m_exitApp = new QAction(tr("退出"),this);
    connect(m_exitApp,SIGNAL(triggered()),this,SLOT(on_exitAppAction()));
    if (Config["muyu_status"].toBool()){
        m_gongde = new QAction(tr("功德：") + QString::number(Config.value("gd").toInteger()),this);
    }/*else{
        m_gongde = new QAction("φ(*￣0￣)",this);
    }*/

}
void MainTableWidget::startMainWindow(){
    MainWindow *mainwin = new MainWindow(this);
    connect(mainwin,&MainWindow::hk,this,&MainTableWidget::hk_slot);
    connect(mainwin,&MainWindow::timerisStart,this,&MainTableWidget::on_timerisStart);
    connect(mainwin,&MainWindow::stopTm,this,&MainTableWidget::on_stopTimer);
    connect(mainwin,&MainWindow::startTm,this,&MainTableWidget::on_startTimer);
    connect(mainwin,&MainWindow::getTimer,this,&MainTableWidget::on_getTimer);
    connect(this,&MainTableWidget::reText,mainwin,&MainWindow::on_reText);
    QScreen *scr = qApp->primaryScreen();
    int scr_w = scr->size().width();
    int scr_h = scr->size().height();
    mainwin->move((scr_w - mainwin->width()) / 2, (scr_h - mainwin->height()) / 3);
    mainwin->setModal(false);
    mainwin->show();
}
void MainTableWidget::startGetStart(){
    GetStartWidget *getstart = new GetStartWidget(this);
    connect(getstart,&GetStartWidget::toSettings,this,&MainTableWidget::on_showConfig_modal);

    QScreen *scr = qApp->primaryScreen();
    int scr_w = scr->size().width();
    int scr_h = scr->size().height();
    getstart->move((scr_w - getstart->width()) / 2, (scr_h - getstart->height()) / 3);
    getstart->setModal(false);
    getstart->show();
    while (getstart->isVisible()){
        QCoreApplication::processEvents(QEventLoop::AllEvents,1145141919810);
    }
}
void MainTableWidget::createMenu(){

    tray_menu = new QMenu(this);
    tray_menu->addAction(m_showedit);
    tray_menu->addAction(m_showmain);
    if (Config["muyu_status"].toBool()){
        tray_menu->addAction(m_gongde);
    }
    tray_menu->addSeparator();
    tray_menu->addAction(m_exitApp);
    m_sysTrayIcon->setContextMenu(tray_menu);
}
void MainTableWidget::on_showMainAction(){

    EditWindow->show();
}
void MainTableWidget::on_showConfig_modal(){
    EditWindow->show();
    while (EditWindow->isVisible()){
        QCoreApplication::processEvents(QEventLoop::AllEvents,1145141919810);
    }
}
void MainTableWidget::on_exitAppAction(){
    qApp->exit();
}
void MainTableWidget::refechTable_slot(){
    rtt->terminate();
    readTimeTable();



    for (QWidget*widget : ui->class_show_widget->findChildren<QWidget*>())
    {
        delete widget;
    }
    rtt->start();
    resize(154+20+today_table.count()*49,height());
    ui->stackedWidget->resize(154+20+today_table.count()*49,height());
    ui->status_show->resize(154+20+today_table.count()*49,height());
    QScreen *scr = qApp->primaryScreen();
    int scr_w = scr->size().width();
    int scr_h = scr->size().height();
    move((scr_w - width()) / 2, 0);
    m_sysTrayIcon->showMessage(tr("提示"),tr("配置已成功应用！"),QSystemTrayIcon::MessageIcon::Information,500);

}


void MainTableWidget::setStyleSheetFromFile(QWidget* widget,QString file){
    QFile styleFile(file);
    if(styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        showLog("StyleSheet is Loaded",LogStatus::INFO);
        QString setStyleSheet(styleFile.readAll());
        widget->setStyleSheet(setStyleSheet);
        styleFile.close();
    }
    else
    {
        showLog("StyleSheet is Load failed",LogStatus::ERR);
    }
}
