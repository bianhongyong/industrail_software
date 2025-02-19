/****************************************************************************
** Meta object code from reading C++ file 'customListwidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.11)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../widgets/customListwidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'customListwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.11. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CustomListWidget_t {
    QByteArrayData data[11];
    char stringdata0[156];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CustomListWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CustomListWidget_t qt_meta_stringdata_CustomListWidget = {
    {
QT_MOC_LITERAL(0, 0, 16), // "CustomListWidget"
QT_MOC_LITERAL(1, 17, 16), // "not_delete_label"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 10), // "label_name"
QT_MOC_LITERAL(4, 46, 12), // "paint_update"
QT_MOC_LITERAL(5, 59, 17), // "delete_annotation"
QT_MOC_LITERAL(6, 77, 16), // "QListWidgetItem*"
QT_MOC_LITERAL(7, 94, 4), // "item"
QT_MOC_LITERAL(8, 99, 22), // "delete_all_annotations"
QT_MOC_LITERAL(9, 122, 13), // "onItemchanged"
QT_MOC_LITERAL(10, 136, 19) // "onItemDoubleClicked"

    },
    "CustomListWidget\0not_delete_label\0\0"
    "label_name\0paint_update\0delete_annotation\0"
    "QListWidgetItem*\0item\0delete_all_annotations\0"
    "onItemchanged\0onItemDoubleClicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CustomListWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x06 /* Public */,
       4,    0,   47,    2, 0x06 /* Public */,
       5,    1,   48,    2, 0x06 /* Public */,
       8,    0,   51,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       9,    1,   52,    2, 0x09 /* Protected */,
      10,    1,   55,    2, 0x09 /* Protected */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 6,    7,
    QMetaType::Void, 0x80000000 | 6,    7,

       0        // eod
};

void CustomListWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CustomListWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->not_delete_label((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->paint_update(); break;
        case 2: _t->delete_annotation((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->delete_all_annotations(); break;
        case 4: _t->onItemchanged((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: _t->onItemDoubleClicked((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CustomListWidget::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomListWidget::not_delete_label)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CustomListWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomListWidget::paint_update)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (CustomListWidget::*)(QListWidgetItem * );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomListWidget::delete_annotation)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (CustomListWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CustomListWidget::delete_all_annotations)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CustomListWidget::staticMetaObject = { {
    &QListWidget::staticMetaObject,
    qt_meta_stringdata_CustomListWidget.data,
    qt_meta_data_CustomListWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CustomListWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CustomListWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CustomListWidget.stringdata0))
        return static_cast<void*>(this);
    return QListWidget::qt_metacast(_clname);
}

int CustomListWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QListWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void CustomListWidget::not_delete_label(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CustomListWidget::paint_update()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void CustomListWidget::delete_annotation(QListWidgetItem * _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CustomListWidget::delete_all_annotations()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
