#ifndef PLUGINWE_H
#define PLUGINWE_H
#include <QWidget>
#include <QWebEngineView>
class PluginWebEngine : public QWebEngineView
{
    Q_OBJECT
public:
    explicit PluginWebEngine(QWidget* parent = nullptr);
protected:
    /**
     * @brief createWindow �������������ʱ��ᴥ���������
     * @param type
     * @return
     */
    QWebEngineView* createWindow(QWebEnginePage::WebWindowType type);
};
#endif // SWEBENGINEVIEW_H