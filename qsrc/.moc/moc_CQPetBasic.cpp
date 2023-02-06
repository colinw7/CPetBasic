/****************************************************************************
** Meta object code from reading C++ file 'CQPetBasic.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQPetBasic.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQPetBasic.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQPetBasicApp_t {
    QByteArrayData data[5];
    char stringdata0[43];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQPetBasicApp_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQPetBasicApp_t qt_meta_stringdata_CQPetBasicApp = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CQPetBasicApp"
QT_MOC_LITERAL(1, 14, 14), // "executeCommand"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 3), // "cmd"
QT_MOC_LITERAL(4, 34, 8) // "keyPress"

    },
    "CQPetBasicApp\0executeCommand\0\0cmd\0"
    "keyPress"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQPetBasicApp[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x0a /* Public */,
       4,    1,   27,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

void CQPetBasicApp::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQPetBasicApp *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->executeCommand((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->keyPress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQPetBasicApp::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CQPetBasicApp.data,
    qt_meta_data_CQPetBasicApp,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQPetBasicApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQPetBasicApp::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQPetBasicApp.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CQPetBasicApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}
struct qt_meta_stringdata_CQPetBasicTerm_t {
    QByteArrayData data[3];
    char stringdata0[30];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQPetBasicTerm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQPetBasicTerm_t qt_meta_stringdata_CQPetBasicTerm = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CQPetBasicTerm"
QT_MOC_LITERAL(1, 15, 13), // "cursorTimeout"
QT_MOC_LITERAL(2, 29, 0) // ""

    },
    "CQPetBasicTerm\0cursorTimeout\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQPetBasicTerm[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CQPetBasicTerm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQPetBasicTerm *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->cursorTimeout(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQPetBasicTerm::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CQPetBasicTerm.data,
    qt_meta_data_CQPetBasicTerm,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQPetBasicTerm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQPetBasicTerm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQPetBasicTerm.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CQPetBasicTerm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
struct qt_meta_stringdata_CQPetBasicCommandScroll_t {
    QByteArrayData data[1];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQPetBasicCommandScroll_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQPetBasicCommandScroll_t qt_meta_stringdata_CQPetBasicCommandScroll = {
    {
QT_MOC_LITERAL(0, 0, 23) // "CQPetBasicCommandScroll"

    },
    "CQPetBasicCommandScroll"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQPetBasicCommandScroll[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void CQPetBasicCommandScroll::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQPetBasicCommandScroll::staticMetaObject = { {
    QMetaObject::SuperData::link<CQCommand::ScrollArea::staticMetaObject>(),
    qt_meta_stringdata_CQPetBasicCommandScroll.data,
    qt_meta_data_CQPetBasicCommandScroll,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQPetBasicCommandScroll::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQPetBasicCommandScroll::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQPetBasicCommandScroll.stringdata0))
        return static_cast<void*>(this);
    return CQCommand::ScrollArea::qt_metacast(_clname);
}

int CQPetBasicCommandScroll::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQCommand::ScrollArea::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_CQPetBasicCommandTerm_t {
    QByteArrayData data[1];
    char stringdata0[22];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQPetBasicCommandTerm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQPetBasicCommandTerm_t qt_meta_stringdata_CQPetBasicCommandTerm = {
    {
QT_MOC_LITERAL(0, 0, 21) // "CQPetBasicCommandTerm"

    },
    "CQPetBasicCommandTerm"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQPetBasicCommandTerm[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

void CQPetBasicCommandTerm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQPetBasicCommandTerm::staticMetaObject = { {
    QMetaObject::SuperData::link<CQCommand::CommandWidget::staticMetaObject>(),
    qt_meta_stringdata_CQPetBasicCommandTerm.data,
    qt_meta_data_CQPetBasicCommandTerm,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQPetBasicCommandTerm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQPetBasicCommandTerm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQPetBasicCommandTerm.stringdata0))
        return static_cast<void*>(this);
    return CQCommand::CommandWidget::qt_metacast(_clname);
}

int CQPetBasicCommandTerm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQCommand::CommandWidget::qt_metacall(_c, _id, _a);
    return _id;
}
struct qt_meta_stringdata_CQPetBasicStatus_t {
    QByteArrayData data[3];
    char stringdata0[27];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQPetBasicStatus_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQPetBasicStatus_t qt_meta_stringdata_CQPetBasicStatus = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CQPetBasicStatus"
QT_MOC_LITERAL(1, 17, 8), // "stopSlot"
QT_MOC_LITERAL(2, 26, 0) // ""

    },
    "CQPetBasicStatus\0stopSlot\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQPetBasicStatus[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CQPetBasicStatus::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQPetBasicStatus *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->stopSlot(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQPetBasicStatus::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQPetBasicStatus.data,
    qt_meta_data_CQPetBasicStatus,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQPetBasicStatus::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQPetBasicStatus::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQPetBasicStatus.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQPetBasicStatus::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
