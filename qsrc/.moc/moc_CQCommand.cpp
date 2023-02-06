/****************************************************************************
** Meta object code from reading C++ file 'CQCommand.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQCommand.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQCommand.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQCommand__ScrollArea_t {
    QByteArrayData data[5];
    char stringdata0[60];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCommand__ScrollArea_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCommand__ScrollArea_t qt_meta_stringdata_CQCommand__ScrollArea = {
    {
QT_MOC_LITERAL(0, 0, 21), // "CQCommand::ScrollArea"
QT_MOC_LITERAL(1, 22, 14), // "executeCommand"
QT_MOC_LITERAL(2, 37, 0), // ""
QT_MOC_LITERAL(3, 38, 8), // "keyPress"
QT_MOC_LITERAL(4, 47, 12) // "updateScroll"

    },
    "CQCommand::ScrollArea\0executeCommand\0"
    "\0keyPress\0updateScroll"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCommand__ScrollArea[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x06 /* Public */,
       3,    1,   32,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    0,   35,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void CQCommand::ScrollArea::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ScrollArea *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->executeCommand((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->keyPress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->updateScroll(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (ScrollArea::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScrollArea::executeCommand)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (ScrollArea::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&ScrollArea::keyPress)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQCommand::ScrollArea::staticMetaObject = { {
    QMetaObject::SuperData::link<CQScrollArea::staticMetaObject>(),
    qt_meta_stringdata_CQCommand__ScrollArea.data,
    qt_meta_data_CQCommand__ScrollArea,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQCommand::ScrollArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQCommand::ScrollArea::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQCommand__ScrollArea.stringdata0))
        return static_cast<void*>(this);
    return CQScrollArea::qt_metacast(_clname);
}

int CQCommand::ScrollArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQScrollArea::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void CQCommand::ScrollArea::executeCommand(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CQCommand::ScrollArea::keyPress(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
struct qt_meta_stringdata_CQCommand__CommandWidget_t {
    QByteArrayData data[13];
    char stringdata0[157];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCommand__CommandWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCommand__CommandWidget_t qt_meta_stringdata_CQCommand__CommandWidget = {
    {
QT_MOC_LITERAL(0, 0, 24), // "CQCommand::CommandWidget"
QT_MOC_LITERAL(1, 25, 14), // "executeCommand"
QT_MOC_LITERAL(2, 40, 0), // ""
QT_MOC_LITERAL(3, 41, 8), // "keyPress"
QT_MOC_LITERAL(4, 50, 9), // "scrollEnd"
QT_MOC_LITERAL(5, 60, 8), // "copySlot"
QT_MOC_LITERAL(6, 69, 9), // "pasteSlot"
QT_MOC_LITERAL(7, 79, 22), // "completionSelectedSlot"
QT_MOC_LITERAL(8, 102, 3), // "str"
QT_MOC_LITERAL(9, 106, 23), // "completionCancelledSlot"
QT_MOC_LITERAL(10, 130, 6), // "prompt"
QT_MOC_LITERAL(11, 137, 8), // "minLines"
QT_MOC_LITERAL(12, 146, 10) // "showPrompt"

    },
    "CQCommand::CommandWidget\0executeCommand\0"
    "\0keyPress\0scrollEnd\0copySlot\0pasteSlot\0"
    "completionSelectedSlot\0str\0"
    "completionCancelledSlot\0prompt\0minLines\0"
    "showPrompt"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCommand__CommandWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       3,   62, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x06 /* Public */,
       3,    1,   52,    2, 0x06 /* Public */,
       4,    0,   55,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    0,   56,    2, 0x08 /* Private */,
       6,    0,   57,    2, 0x08 /* Private */,
       7,    1,   58,    2, 0x08 /* Private */,
       9,    0,   61,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    8,
    QMetaType::Void,

 // properties: name, type, flags
      10, QMetaType::QString, 0x00095103,
      11, QMetaType::Int, 0x00095103,
      12, QMetaType::Bool, 0x00095103,

       0        // eod
};

void CQCommand::CommandWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CommandWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->executeCommand((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->keyPress((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->scrollEnd(); break;
        case 3: _t->copySlot(); break;
        case 4: _t->pasteSlot(); break;
        case 5: _t->completionSelectedSlot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->completionCancelledSlot(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CommandWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWidget::executeCommand)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CommandWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWidget::keyPress)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CommandWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CommandWidget::scrollEnd)) {
                *result = 2;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CommandWidget *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->prompt(); break;
        case 1: *reinterpret_cast< int*>(_v) = _t->minLines(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isShowPrompt(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CommandWidget *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setPrompt(*reinterpret_cast< QString*>(_v)); break;
        case 1: _t->setMinLines(*reinterpret_cast< int*>(_v)); break;
        case 2: _t->setShowPrompt(*reinterpret_cast< bool*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject CQCommand::CommandWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQCommand__CommandWidget.data,
    qt_meta_data_CQCommand__CommandWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQCommand::CommandWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQCommand::CommandWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQCommand__CommandWidget.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQCommand::CommandWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void CQCommand::CommandWidget::executeCommand(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CQCommand::CommandWidget::keyPress(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CQCommand::CommandWidget::scrollEnd()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}
struct qt_meta_stringdata_CQCommand__CompletionList_t {
    QByteArrayData data[4];
    char stringdata0[54];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQCommand__CompletionList_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQCommand__CompletionList_t qt_meta_stringdata_CQCommand__CompletionList = {
    {
QT_MOC_LITERAL(0, 0, 25), // "CQCommand::CompletionList"
QT_MOC_LITERAL(1, 26, 12), // "itemSelected"
QT_MOC_LITERAL(2, 39, 0), // ""
QT_MOC_LITERAL(3, 40, 13) // "itemCancelled"

    },
    "CQCommand::CompletionList\0itemSelected\0"
    "\0itemCancelled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQCommand__CompletionList[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   24,    2, 0x06 /* Public */,
       3,    0,   27,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,

       0        // eod
};

void CQCommand::CompletionList::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CompletionList *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->itemSelected((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->itemCancelled(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CompletionList::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CompletionList::itemSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CompletionList::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CompletionList::itemCancelled)) {
                *result = 1;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQCommand::CompletionList::staticMetaObject = { {
    QMetaObject::SuperData::link<QListWidget::staticMetaObject>(),
    qt_meta_stringdata_CQCommand__CompletionList.data,
    qt_meta_data_CQCommand__CompletionList,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQCommand::CompletionList::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQCommand::CompletionList::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQCommand__CompletionList.stringdata0))
        return static_cast<void*>(this);
    return QListWidget::qt_metacast(_clname);
}

int CQCommand::CompletionList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void CQCommand::CompletionList::itemSelected(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CQCommand::CompletionList::itemCancelled()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
