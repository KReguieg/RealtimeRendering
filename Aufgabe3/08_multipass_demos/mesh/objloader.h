/****************************************************************************
**
** Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company.
** Author: Giuseppe D'Angelo
** Contact: info@kdab.com
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

/* modified by Hartmut Schirmacher: replaced QVector by std::vector */

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <vector> // std::vector

#include <limits>

class QString;
class QIODevice;

struct FaceIndices;

class ObjLoader
{
public:
    ObjLoader();

    void setLoadTextureCoordinatesEnabled( bool b ) { m_loadTextureCoords = b; }
    bool isLoadTextureCoordinatesEnabled() const { return m_loadTextureCoords; }

    void setMeshCenteringEnabled( bool b ) { m_centerMesh = b; }
    bool isMeshCenteringEnabled() const { return m_centerMesh; }

    bool hasNormals() const { return !m_normals.empty(); }
    bool hasTextureCoordinates() const { return !m_texCoords.empty(); }

    bool load( const QString& fileName );
    bool load( QIODevice* ioDev );

    std::vector<QVector3D> vertices() const { return m_points; }
    std::vector<QVector3D> normals() const { return m_normals; }
    std::vector<QVector2D> textureCoordinates() const { return m_texCoords; }
    std::vector<unsigned int> indices() const { return m_indices; }

private:
    void updateIndices(const std::vector<QVector3D> &positions,
                       const std::vector<QVector3D> &normals,
                       const std::vector<QVector2D> &texCoords,
                       const QHash<FaceIndices, unsigned int> &faceIndexMap,
                       const std::vector<FaceIndices> &faceIndexVector);
    void generateAveragedNormals( const std::vector<QVector3D>& points,
                                  std::vector<QVector3D>& normals,
                                  const std::vector<unsigned int>& faces ) const;
    void center( std::vector<QVector3D>& points );

    bool m_loadTextureCoords;
    // bool m_generateTangents;
    bool m_centerMesh;

    std::vector<QVector3D> m_points;
    std::vector<QVector3D> m_normals;
    std::vector<QVector2D> m_texCoords;
    std::vector<unsigned int> m_indices;
};

#endif // OBJLOADER_H
