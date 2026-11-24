#pragma once

#include <QtCore/qglobal.h>

#if defined(GLTFLOADER_LIBRARY)
#define GLTFLOADER_EXPORT Q_DECL_EXPORT
#else
#define GLTFLOADER_EXPORT Q_DECL_IMPORT
#endif
