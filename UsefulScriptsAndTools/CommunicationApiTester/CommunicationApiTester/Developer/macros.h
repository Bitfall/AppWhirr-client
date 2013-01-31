#ifndef MACROS_H
#define MACROS_H

//
// --- math ---
#include <math.h>
inline double awRoundd(double x) { return floor(x + 0.5); }
inline float awRoundf(float x) { return floor(x + 0.5f); }

//
// --- utility ---

//#define AW_QOBJECT_SAFE_DELETE(x)   if(x != NULL) { QObject *qX = dynamic_cast<QObject *>(x); if(qX != NULL) { qX->setParent(NULL); qX->deleteLater(); } else { delete x; } x = NULL; }

#define AW_SAFE_DELETE(x)           if(x != NULL) { delete x; x = NULL; }
#define AW_QOBJECT_SAFE_DELETE(x)   if(x != NULL) { x->setParent(NULL); x->deleteLater(); x = NULL; }

//#define AW_QOBJECT_SAFE_DELETE(x) if(x != NULL) { qX = dynamic_cast<QObject *>(x); if(qX != NULL) { qX->setParent(NULL); } delete x; x = NULL; }
//#define AW_SAFE_DELETE_LATER(x) if(x != NULL) { qX = dynamic_cast<QObject *>(x); if(qX != NULL) { qX->setParent(NULL); } delete x; x = NULL; }
//#define AW_QOBJECT_SAFE_DELETE(x) if(x != NULL) { x->setParent(NULL); delete x; x = NULL; }
//#define AW_QOBJECT_SAFE_DELETE_LATER(x) if(x != NULL) { x->setParent(NULL); x->deleteLater(); x = NULL; }

//
// --- logging ---
#include <QDebug>

#ifdef DEBUG
    #ifdef ENABLE_DEBUG_ASSERT_LOG_ASSERTATION
        #define ALog(exp)                   { qDebug() << __FILE__ << "(" << __LINE__ << ") /" << __FUNCTION__  << ":" << exp; exit(EXIT_FAILURE); }
    #else
        #define ALog(exp)                   { qDebug() << __FILE__ << "(" << __LINE__ << ") /" << __FUNCTION__  << ":" << exp; }
    #endif
#define DLog(exp)                   qDebug() << __FUNCTION__  << ":" << exp
#define DLogS                       qDebug() << __FUNCTION__  << ":"
#define WLog(exp)                   qDebug() << " [!warning!] " << __FUNCTION__  << ":" << exp
#define WLogS                       qDebug() << " [!warning!] " << __FUNCTION__  << ":"
    #ifdef ENABLE_DEPRECATED_WARNING_LOGGING
        #define FLAG_AS_DEPRECATED          DLog("[deprecated]")
    #else
        #define FLAG_AS_DEPRECATED
    #endif
#define LOG_AS_TMP_OFF                              qDebug() << "[!tmp off!] " << __FUNCTION__
#define LOG_AS_TMP_OFF_WITH_HINT(hint)              qDebug() << "[!tmp off!] " << __FUNCTION__  << ":" << hint
#define LOG_AS_NOT_IMPLEMENTED                      qDebug() << "[not implemented] " << __FUNCTION__
#define LOG_AS_NOT_IMPLEMENTED_WITH_HINT(hint)      qDebug() << "[not implemented] " << __FUNCTION__  << ":" << hint
    #ifdef ENABLE_REVIEW_THIS_LOGGING
        #define FLAG_FOR_REVIEW_WITH_HINT(hint)             qDebug() << "[!review this!] " << __FUNCTION__  << ":" << hint
    #else
        #define FLAG_FOR_REVIEW_WITH_HINT(hint)
    #endif
#else
// Todo: implement our logging system (html)
// note: qDebug is turned off in the .pro file if we're in Debug mode, so it's safe to use it, it won't do anything
//      but this is required because you can use e.g. DLogS << some << shot, which could lead to problems
#define DLog(exp)                                   qDebug()
#define DLogS                                       qDebug()
#define WLog(exp)                                   qDebug()
#define WLogS                                       qDebug()
#define LOG_AS_TMP_OFF
#define LOG_AS_TMP_OFF_WITH_HINT(hint)              qDebug()
#define LOG_AS_NOT_IMPLEMENTED                      qDebug()
#define LOG_AS_NOT_IMPLEMENTED_WITH_HINT(hint)      qDebug()
#define FLAG_FOR_REVIEW_WITH_HINT(hint)             qDebug()
#define FLAG_AS_DEPRECATED
#define ALog(exp)                                   qDebug()
#endif

#endif // MACROS_H
