/****************************************************************************
** Meta object code from reading C++ file 'CQPetBasicDbg.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQPetBasicDbg.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQPetBasicDbg.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQPetBasicDbg_t {
    QByteArrayData data[14];
    char stringdata0[153];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQPetBasicDbg_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQPetBasicDbg_t qt_meta_stringdata_CQPetBasicDbg = {
    {
QT_MOC_LITERAL(0, 0, 13), // "CQPetBasicDbg"
QT_MOC_LITERAL(1, 14, 16), // "updateFileOffset"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 14), // "updateFileSize"
QT_MOC_LITERAL(4, 47, 8), // "playSlot"
QT_MOC_LITERAL(5, 56, 9), // "pauseSlot"
QT_MOC_LITERAL(6, 66, 8), // "stepSlot"
QT_MOC_LITERAL(7, 75, 7), // "bgColor"
QT_MOC_LITERAL(8, 83, 7), // "fgColor"
QT_MOC_LITERAL(9, 91, 12), // "keywordColor"
QT_MOC_LITERAL(10, 104, 11), // "stringColor"
QT_MOC_LITERAL(11, 116, 13), // "operatorColor"
QT_MOC_LITERAL(12, 130, 9), // "markColor"
QT_MOC_LITERAL(13, 140, 12) // "currentColor"

    },
    "CQPetBasicDbg\0updateFileOffset\0\0"
    "updateFileSize\0playSlot\0pauseSlot\0"
    "stepSlot\0bgColor\0fgColor\0keywordColor\0"
    "stringColor\0operatorColor\0markColor\0"
    "currentColor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQPetBasicDbg[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       7,   46, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    1,   40,    2, 0x08 /* Private */,
       4,    0,   43,    2, 0x08 /* Private */,
       5,    0,   44,    2, 0x08 /* Private */,
       6,    0,   45,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::QSize,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // properties: name, type, flags
       7, QMetaType::QColor, 0x00095103,
       8, QMetaType::QColor, 0x00095103,
       9, QMetaType::QColor, 0x00095103,
      10, QMetaType::QColor, 0x00095103,
      11, QMetaType::QColor, 0x00095103,
      12, QMetaType::QColor, 0x00095103,
      13, QMetaType::QColor, 0x00095103,

       0        // eod
};

void CQPetBasicDbg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQPetBasicDbg *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateFileOffset(); break;
        case 1: _t->updateFileSize((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        case 2: _t->playSlot(); break;
        case 3: _t->pauseSlot(); break;
        case 4: _t->stepSlot(); break;
        default: ;
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CQPetBasicDbg *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->bgColor(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = _t->fgColor(); break;
        case 2: *reinterpret_cast< QColor*>(_v) = _t->keywordColor(); break;
        case 3: *reinterpret_cast< QColor*>(_v) = _t->stringColor(); break;
        case 4: *reinterpret_cast< QColor*>(_v) = _t->operatorColor(); break;
        case 5: *reinterpret_cast< QColor*>(_v) = _t->markColor(); break;
        case 6: *reinterpret_cast< QColor*>(_v) = _t->currentColor(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CQPetBasicDbg *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setBgColor(*reinterpret_cast< QColor*>(_v)); break;
        case 1: _t->setFgColor(*reinterpret_cast< QColor*>(_v)); break;
        case 2: _t->setKeywordColor(*reinterpret_cast< QColor*>(_v)); break;
        case 3: _t->setStringColor(*reinterpret_cast< QColor*>(_v)); break;
        case 4: _t->setOperatorColor(*reinterpret_cast< QColor*>(_v)); break;
        case 5: _t->setMarkColor(*reinterpret_cast< QColor*>(_v)); break;
        case 6: _t->setCurrentColor(*reinterpret_cast< QColor*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject CQPetBasicDbg::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQPetBasicDbg.data,
    qt_meta_data_CQPetBasicDbg,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQPetBasicDbg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQPetBasicDbg::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQPetBasicDbg.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQPetBasicDbg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_CQPetBasicFileView_t {
    QByteArrayData data[3];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQPetBasicFileView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQPetBasicFileView_t qt_meta_stringdata_CQPetBasicFileView = {
    {
QT_MOC_LITERAL(0, 0, 18), // "CQPetBasicFileView"
QT_MOC_LITERAL(1, 19, 10), // "updateSize"
QT_MOC_LITERAL(2, 30, 0) // ""

    },
    "CQPetBasicFileView\0updateSize\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQPetBasicFileView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   19,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QSize,    2,

       0        // eod
};

void CQPetBasicFileView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQPetBasicFileView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->updateSize((*reinterpret_cast< const QSize(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQPetBasicFileView::*)(const QSize & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQPetBasicFileView::updateSize)) {
                *result = 0;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CQPetBasicFileView::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQPetBasicFileView.data,
    qt_meta_data_CQPetBasicFileView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQPetBasicFileView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQPetBasicFileView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQPetBasicFileView.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQPetBasicFileView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void CQPetBasicFileView::updateSize(const QSize & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
