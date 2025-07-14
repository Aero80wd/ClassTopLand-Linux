#include "GlassHelper.h"
#include <QWidget>
#include <QDebug>

#include <qguiapplication.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

void GlassHelper::enableBlurBehind(QWidget *widget) {
    QString platform = QGuiApplication::platformName();
    if (platform == "xcb" || platform == "x11") {
        if (!widget->winId()) {
            qWarning() << "Widget has no winId!";
            return;
        }

        Display *display = XOpenDisplay(nullptr);
        if (!display) {
            qWarning() << "Failed to open X display.";
            return;
        }

        Window window = static_cast<Window>(widget->winId());

        Atom blurAtom = XInternAtom(display, "_KDE_NET_WM_BLUR_BEHIND_REGION", False);
        if (blurAtom == None) {
            qWarning() << "_KDE_NET_WM_BLUR_BEHIND_REGION atom not found.";
            XCloseDisplay(display);
            return;
        }

        unsigned long blurValue = 1;
        XChangeProperty(display,
                        window,
                        blurAtom,
                        XA_CARDINAL,
                        32,
                        PropModeReplace,
                        reinterpret_cast<unsigned char*>(&blurValue),
                        1);

        XFlush(display);
        XCloseDisplay(display);
    }else {
        qWarning() << "Unsupported platform for blur behind.";
    }
}