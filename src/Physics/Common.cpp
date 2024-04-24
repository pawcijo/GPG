#include "Common.h"
#include "Plane.h"

#include <cmath>

#include <cstdio>

#define CMP(x, y) (fabsf(x - y) <= FLT_EPSILON * fmaxf(1.0f, fmaxf(fabsf(x), fabsf(y))))

float Magnitude(const glm::vec2 &v)
{
    return sqrtf(glm::dot(v, v));
}

float Magnitude(const glm::vec3 &v)
{
    return sqrtf(glm::dot(v, v));
}

glm::vec3 GetMin(const AABB &aabb)
{
    glm::vec3 p1 = aabb.position + aabb.size;
    glm::vec3 p2 = aabb.position - aabb.size;

    return glm::vec3(fminf(p1.x, p2.x), fminf(p1.y, p2.y), fminf(p1.z, p2.z));
}
glm::vec3 GetMax(const AABB &aabb)
{
    glm::vec3 p1 = aabb.position + aabb.size;
    glm::vec3 p2 = aabb.position - aabb.size;

    return glm::vec3(fmaxf(p1.x, p2.x), fmaxf(p1.y, p2.y), fmaxf(p1.z, p2.z));
}

AABB FromMinMax(const glm::vec3 &min, const glm::vec3 &max)
{
    return AABB((min + max) * 0.5f, (max - min) * 0.5f);
}

float MagnitudeSq(const glm::vec2 &v)
{
    return glm::dot(v, v);
}

float MagnitudeSq(const glm::vec3 &v)
{
    return glm::dot(v, v);
}

bool SphereSphere(const Sphere &s1, const Sphere &s2)
{
    float radiiSum = s1.radius + s2.radius;
    float sqDistance = MagnitudeSq(s1.position - s2.position);
    return sqDistance < radiiSum * radiiSum;
}

bool ContainsPoint(const AABB &aabb, const glm::vec3 &point)
{
    return PointInAABB(point, aabb);
}

bool ContainsPoint(const glm::vec3 &point, const AABB &aabb)
{
    return PointInAABB(point, aabb);
}

bool AABBAABB(const AABB &aabb1, const AABB &aabb2)
{
    glm::vec3 aMin = GetMin(aabb1);
    glm::vec3 aMax = GetMax(aabb1);
    glm::vec3 bMin = GetMin(aabb2);
    glm::vec3 bMax = GetMax(aabb2);

    return (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
           (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
           (aMin.z <= bMax.z && aMax.z >= bMin.z);
}

glm::vec3 MultiplyVector(const glm::vec3 &vec, const glm::mat4 &mat)
{
    glm::vec3 result;

    // TODO:: make sure if values are right
    // printf("MultiplyVector mat4 \n");
    result.x = vec.x * mat[0][0] + vec.y * mat[1][0] + vec.z * mat[2][0] + 0.0f * mat[3][0];
    result.y = vec.x * mat[0][1] + vec.y * mat[1][2] + vec.z * mat[2][1] + 0.0f * mat[3][1];
    result.z = vec.x * mat[0][2] + vec.y * mat[1][2] + vec.z * mat[2][2] + 0.0f * mat[3][2];
    // printf("MultiplyVector mat4 end \n");
    return result;
}

glm::vec3 MultiplyVector(const glm::vec3 &vec, const glm::mat3 &mat)
{
    glm::vec3 result;
    // TODO:: make sure if values are right
    // printf("MultiplyVector mat3");
    result.x = glm::dot(vec, glm::vec3{mat[0][0], mat[1][0], mat[2][0]});
    result.y = glm::dot(vec, glm::vec3{mat[0][1], mat[1][1], mat[2][1]});
    result.z = glm::dot(vec, glm::vec3{mat[0][2], mat[1][2], mat[2][2]});
    // printf("MultiplyVector mat3 end");
    return result;
}

void ResetCollisionResult(CollisionResult *result)
{
    if (result != 0)
    {
        result->colliding = false;
        result->normal = glm::vec3(0, 0, 1);
        result->depth = FLT_MAX;
        if (result->contacts.size() > 0)
        {
            result->contacts.clear();
        }
    }
}

float PenetrationDepth(const OBB &o1, const OBB &o2, const glm::vec3 &axis, bool *outShouldFlip)
{
    Interval i1 = GetInterval(o1, glm::normalize(axis));
    Interval i2 = GetInterval(o2, glm::normalize(axis));

    if (!((i2.min <= i1.max) && (i1.min <= i2.max)))
    {
        return 0.0f; // No penerattion
    }

    float len1 = i1.max - i1.min;
    float len2 = i2.max - i2.min;
    float min = fminf(i1.min, i2.min);
    float max = fmaxf(i1.max, i2.max);
    float length = max - min;

    if (outShouldFlip != 0)
    {
        *outShouldFlip = (i2.min < i1.min);
    }

    return (len1 + len2) - length;
}

CollisionResult FindCollisionFeatures(const Sphere &A, const Sphere &B)
{
    CollisionResult result; // Will return result of intersection!
    ResetCollisionResult(&result);

    float r = A.radius + B.radius;
    glm::vec3 d = B.position - A.position;

    if (MagnitudeSq(d) - r * r > 0 || MagnitudeSq(d) == 0.0f)
    {
        return result;
    }
    d = glm::normalize(d);

    result.colliding = true;
    result.normal = d;
    result.depth = fabsf(Magnitude(d) - r) * 0.5f;

    // dtp - Distance to intersection point
    float dtp = A.radius - result.depth;
    glm::vec3 contact = A.position + d * dtp;

    result.contacts.push_back(contact);

    return result;
}

CollisionResult FindCollisionFeatures(const OBB &A, const Sphere &B)
{
    CollisionResult result; // Will return result of intersection!
    ResetCollisionResult(&result);

    glm::vec3 closestPoint = ClosestPoint(A, B.position);

    float distanceSq = MagnitudeSq(closestPoint - B.position);
    if (distanceSq > B.radius * B.radius)
    {
        return result;
    }

    glm::vec3 normal;
    if (CMP(distanceSq, 0.0f))
    {
        if (CMP(MagnitudeSq(closestPoint - A.position), 0.0f))
        {
            return result;
        }
        // Closest point is at the center of the sphere
        normal = glm::normalize(closestPoint - A.position);
    }
    else
    {
        normal = glm::normalize(B.position - closestPoint);
    }

    glm::vec3 outsidePoint = B.position - normal * B.radius;

    float distance = Magnitude(closestPoint - outsidePoint);

    result.colliding = true;
    result.contacts.push_back(closestPoint + (outsidePoint - closestPoint) * 0.5f);
    result.normal = normal;
    result.depth = distance * 0.5f;

    return result;
}

CollisionResult FindCollisionFeatures(const OBB &A, const OBB &B)
{
    
    CollisionResult result; // Will return result of intersection!
    ResetCollisionResult(&result);

    Sphere s1(A.position, Magnitude(A.size));
    Sphere s2(B.position, Magnitude(B.size));

    if (!SphereSphere(s1, s2))
    {
        return result;
    }

    const float *o1 = &(A.orientation[0][0]);
    const float *o2 = &(B.orientation[0][0]);

    // printf("FindCollisionFeatures test 226  \n");

    glm::vec3 test[15] = {
        glm::vec3(o1[0], o1[1], o1[2]),
        glm::vec3(o1[3], o1[4], o1[5]),
        glm::vec3(o1[6], o1[7], o1[8]),
        glm::vec3(o2[0], o2[1], o2[2]),
        glm::vec3(o2[3], o2[4], o2[5]),
        glm::vec3(o2[6], o2[7], o2[8])};

    // printf("FindCollisionFeatures cross 237  \n");
    for (int i = 0; i < 3; ++i)
    { // Fill out rest of axis
        test[6 + i * 3 + 0] = glm::cross(test[i], test[0]);
        test[6 + i * 3 + 1] = glm::cross(test[i], test[1]);
        test[6 + i * 3 + 2] = glm::cross(test[i], test[2]);
    }

    glm::vec3 *hitNormal = 0;
    bool shouldFlip;

    for (int i = 0; i < 15; ++i)
    {
        if (test[i].x < 0.000001f)
            test[i].x = 0.0f;
        if (test[i].y < 0.000001f)
            test[i].y = 0.0f;
        if (test[i].z < 0.000001f)
            test[i].z = 0.0f;
        if (MagnitudeSq(test[i]) < 0.001f)
        {
            continue;
        }

        // printf("PenetrationDepth 261 [%i] \n", i);
        float depth = PenetrationDepth(A, B, test[i], &shouldFlip);
        if (depth <= 0.0f)
        {
            return result;
        }
        else if (depth < result.depth)
        {
            if (shouldFlip)
            {
                test[i] = test[i] * -1.0f;
            }
            result.depth = depth;
            hitNormal = &test[i];
        }
    }

    if (hitNormal == 0)
    {
        return result;
    }
    glm::vec3 axis = glm::normalize(*hitNormal);

    // printf("ClipEdgesToOBB  284\n");
    std::vector<glm::vec3> c1 = ClipEdgesToOBB(GetEdges(B), A);
    std::vector<glm::vec3> c2 = ClipEdgesToOBB(GetEdges(A), B);
    result.contacts.reserve(c1.size() + c2.size());
    result.contacts.insert(result.contacts.end(), c1.begin(), c1.end());
    result.contacts.insert(result.contacts.end(), c2.begin(), c2.end());

    // printf("GetInterval  end 312\n");
    Interval i = GetInterval(A, axis);
    float distance = (i.max - i.min) * 0.5f - result.depth * 0.5f;
    glm::vec3 pointOnPlane = A.position + axis * distance;

    for (int i = result.contacts.size() - 1; i >= 0; --i)
    {
        glm::vec3 contact = result.contacts[i];
        result.contacts[i] = contact + (axis * glm::dot(axis, pointOnPlane - contact));

        // This bit is in the "There is more" section of the book
        for (int j = result.contacts.size() - 1; j > i; --j)
        {
            if (MagnitudeSq(result.contacts[j] - result.contacts[i]) < 0.0001f)
            {
                result.contacts.erase(result.contacts.begin() + j);
                break;
            }
        }
    }

    result.colliding = true;
    result.normal = axis;

    // printf("FindCollisionFeatures  end 312\n");

    return result;
}
std::vector<Plane> GetPlanes(const OBB &obb)
{
    glm::vec3 c = obb.position; // OBB Center
    glm::vec3 e = obb.size;     // OBB Extents
    const float *o = &(obb.orientation[0][0]);
    glm::vec3 a[] = {
        // OBB Axis
        glm::vec3(o[0], o[1], o[2]),
        glm::vec3(o[3], o[4], o[5]),
        glm::vec3(o[6], o[7], o[8]),
    };

    std::vector<Plane> result;
    result.resize(6);

    result[0] = Plane(a[0], glm::dot(a[0], (c + a[0] * e.x)));
    result[1] = Plane(a[0] * -1.0f, -glm::dot(a[0], (c - a[0] * e.x)));
    result[2] = Plane(a[1], glm::dot(a[1], (c + a[1] * e.y)));
    result[3] = Plane(a[1] * -1.0f, -glm::dot(a[1], (c - a[1] * e.y)));
    result[4] = Plane(a[2], glm::dot(a[2], (c + a[2] * e.z)));
    result[5] = Plane(a[2] * -1.0f, -glm::dot(a[2], (c - a[2] * e.z)));

    return result;
}

bool ClipToPlane(const Plane &plane, const Line &line, glm::vec3 *outPoint)
{

    // printf("ClipToPlane  346\n");
    glm::vec3 ab = line.end - line.start;

    float nA = glm::dot(plane.normal, line.start);
    float nAB = glm::dot(plane.normal, ab);
    // printf("after dot  351\n");

    // printf("CMP  353\n");
    if (CMP(nAB, 0))
    {
        return false;
    }

    float t = (plane.distance - nA) / nAB;
    if (t >= 0.0f && t <= 1.0f)
    {
        if (outPoint != 0)
        {
            *outPoint = line.start + ab * t;
        }
        return true;
    }

    return false;
}

bool PointInSphere(const glm::vec3 &point, const Sphere &sphere)
{
    return MagnitudeSq(point - sphere.position) < sphere.radius * sphere.radius;
}
bool PointInAABB(const glm::vec3 &point, const AABB &aabb)
{
    glm::vec3 min = GetMin(aabb);
    glm::vec3 max = GetMax(aabb);

    if (point.x < min.x || point.y < min.y || point.z < min.z)
    {
        return false;
    }
    if (point.x > max.x || point.y > max.y || point.z > max.z)
    {
        return false;
    }

    return true;
}
bool PointInOBB(const glm::vec3 &point, const OBB &obb)
{
    //!!! Modified with chat gpt !!!
    // Transform the point into the local coordinate system of the OBB
    glm::vec3 localPoint = glm::inverse(obb.orientation) * (glm::vec3(point.x, point.y, point.z) - obb.position);

    // Check if the transformed point lies within the OBB extents
    return (abs(localPoint.x) <= obb.size.x &&
            abs(localPoint.y) <= obb.size.y &&
            abs(localPoint.z) <= obb.size.z);

    return true;
}

std::vector<glm::vec3> ClipEdgesToOBB(const std::vector<Line> &edges, const OBB &obb)
{

    // printf("ClipEdgesToOBB  416\n");
    std::vector<glm::vec3> result;
    result.reserve(edges.size() * 3);
    glm::vec3 intersection;

    // printf("GetPlanes  421\n");
    const std::vector<Plane> &planes = GetPlanes(obb);

    for (int i = 0; i < planes.size(); ++i)
    {
        for (int j = 0; j < edges.size(); ++j)
        {
            if (ClipToPlane(planes[i], edges[j], &intersection))
            {
                if (PointInOBB(intersection, obb))
                {
                    result.push_back(intersection);
                }
            }
        }
    }
    // printf("ClipEdgesToOBB  end\n");
    return result;
}

std::vector<Line> GetEdges(const OBB &obb)
{
    std::vector<Line> result;
    result.reserve(12);
    std::vector<glm::vec3> v = GetVertices(obb);

    int index[][2] = {// Indices of edges
                      {6, 1},
                      {6, 3},
                      {6, 4},
                      {2, 7},
                      {2, 5},
                      {2, 0},
                      {0, 1},
                      {0, 3},
                      {7, 1},
                      {7, 4},
                      {4, 5},
                      {5, 3}};

    for (int j = 0; j < 12; ++j)
    {
        result.push_back(Line(
            v[index[j][0]], v[index[j][1]]));
    }

    return result;
}

std::vector<glm::vec3> GetVertices(const OBB &obb)
{
    std::vector<glm::vec3> v;
    v.resize(8);

    glm::vec3 C = obb.position; // OBB Center
    glm::vec3 E = obb.size;     // OBB Extents
    const float *o = &obb.orientation[0][0];
    glm::vec3 A[] = {
        // OBB Axis
        glm::vec3(o[0], o[1], o[2]),
        glm::vec3(o[3], o[4], o[5]),
        glm::vec3(o[6], o[7], o[8]),
    };

    v[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    v[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    v[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
    v[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    v[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    v[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    v[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    v[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

    return v;
}

Interval GetInterval(const Triangle &triangle, const glm::vec3 &axis)
{
    Interval result;

    result.min = glm::dot(axis, triangle.a);
    result.max = result.min;
    for (int i = 1; i < 3; ++i)
    {
        float value = 0;

        if (i == 0)
        {
            value = glm::dot(axis, triangle.a);
        }
        else if (i == 1)
        {
            value = glm::dot(axis, triangle.b);
        }
        else if (i == 2)
        {
            value = glm::dot(axis, triangle.c);
        }

        result.min = fminf(result.min, value);
        result.max = fmaxf(result.max, value);
    }

    return result;
}

Interval GetInterval(const OBB &obb, const glm::vec3 &axis)
{
    glm::vec3 vertex[8];

    glm::vec3 C = obb.position; // OBB Center
    glm::vec3 E = obb.size;     // OBB Extents
    const float *o = &obb.orientation[0][0];
    glm::vec3 A[] = {
        // OBB Axis
        glm::vec3(o[0], o[1], o[2]),
        glm::vec3(o[3], o[4], o[5]),
        glm::vec3(o[6], o[7], o[8]),
    };

    vertex[0] = C + A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    vertex[1] = C - A[0] * E[0] + A[1] * E[1] + A[2] * E[2];
    vertex[2] = C + A[0] * E[0] - A[1] * E[1] + A[2] * E[2];
    vertex[3] = C + A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    vertex[4] = C - A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    vertex[5] = C + A[0] * E[0] - A[1] * E[1] - A[2] * E[2];
    vertex[6] = C - A[0] * E[0] + A[1] * E[1] - A[2] * E[2];
    vertex[7] = C - A[0] * E[0] - A[1] * E[1] + A[2] * E[2];

    Interval result;
    result.min = result.max = glm::dot(axis, vertex[0]);

    for (int i = 1; i < 8; ++i)
    {
        float projection = glm::dot(axis, vertex[i]);
        result.min = (projection < result.min) ? projection : result.min;
        result.max = (projection > result.max) ? projection : result.max;
    }

    return result;
}

Interval GetInterval(const AABB &aabb, const glm::vec3 &axis)
{
    glm::vec3 i = GetMin(aabb);
    glm::vec3 a = GetMax(aabb);

    glm::vec3 vertex[8] = {
        glm::vec3(i.x, a.y, a.z),
        glm::vec3(i.x, a.y, i.z),
        glm::vec3(i.x, i.y, a.z),
        glm::vec3(i.x, i.y, i.z),
        glm::vec3(a.x, a.y, a.z),
        glm::vec3(a.x, a.y, i.z),
        glm::vec3(a.x, i.y, a.z),
        glm::vec3(a.x, i.y, i.z)};

    Interval result;
    result.min = result.max = glm::dot(axis, vertex[0]);

    for (int i = 1; i < 8; ++i)
    {
        float projection = glm::dot(axis, vertex[i]);
        result.min = (projection < result.min) ? projection : result.min;
        result.max = (projection > result.max) ? projection : result.max;
    }

    return result;
}

glm::mat4 XRotation(float angle)
{
    angle = glm::radians(angle);
    return glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, cosf(angle), sinf(angle), 0.0f,
        0.0f, -sinf(angle), cosf(angle), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

glm::mat3 XRotation3x3(float angle)
{
    angle = glm::radians(angle);
    return glm::mat3(
        1.0f, 0.0f, 0.0f,
        0.0f, cosf(angle), sinf(angle),
        0.0f, -sinf(angle), cosf(angle));
}

glm::mat4 YRotation(float angle)
{
    angle = glm::radians(angle);
    return glm::mat4(
        cosf(angle), 0.0f, -sinf(angle), 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        sinf(angle), 0.0f, cosf(angle), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

glm::mat3 YRotation3x3(float angle)
{
    angle = glm::radians(angle);
    return glm::mat3(
        cosf(angle), 0.0f, -sinf(angle),
        0.0f, 1.0f, 0.0f,
        sinf(angle), 0.0f, cosf(angle));
}

glm::mat4 ZRotation(float angle)
{
    angle = glm::radians(angle);
    return glm::mat4(
        cosf(angle), sinf(angle), 0.0f, 0.0f,
        -sinf(angle), cosf(angle), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

glm::mat3 ZRotation3x3(float angle)
{
    angle = glm::radians(angle);
    return glm::mat3(
        cosf(angle), sinf(angle), 0.0f,
        -sinf(angle), cosf(angle), 0.0f,
        0.0f, 0.0f, 1.0f);
}

glm::mat3 Rotation3x3(float pitch, float yaw, float roll)
{
    return ZRotation3x3(roll) * XRotation3x3(pitch) * YRotation3x3(yaw);
}

Plane FromTriangle(const Triangle &t)
{
    Plane result;
    result.normal = glm::normalize(glm::cross(t.b - t.a, t.c - t.a));
    result.distance = glm::dot(result.normal, t.a);
    return result;
}

bool PointInTriangle(const glm::vec3 &p, const Triangle &t)
{
    // Move the triangle so that the point is
    // now at the origin of the triangle
    glm::vec3 a = t.a - p;
    glm::vec3 b = t.b - p;
    glm::vec3 c = t.c - p;

    // The point should be moved too, so they are both
    // relative, but because we don't use p in the
    // equation anymore, we don't need it!
    // p -= p; // This would just equal the zero vector!

    glm::vec3 normPBC = glm::cross(b, c); // Normal of PBC (u)
    glm::vec3 normPCA = glm::cross(c, a); // Normal of PCA (v)
    glm::vec3 normPAB = glm::cross(a, b); // Normal of PAB (w)

    // Test to see if the normals are facing
    // the same direction, return false if not
    if (glm::dot(normPBC, normPCA) < 0.0f)
    {
        return false;
    }
    else if (glm::dot(normPBC, normPAB) < 0.0f)
    {
        return false;
    }

    // All normals facing the same way, return true
    return true;
}

glm::vec3 ClosestPoint(const Triangle &t, const glm::vec3 &p)
{
    Plane plane = FromTriangle(t);
    glm::vec3 closest = ClosestPoint(plane, p);

    // Closest point was inside triangle
    if (PointInTriangle(closest, t))
    {
        return closest;
    }

    glm::vec3 c1 = ClosestPoint(Line(t.a, t.b), closest); // Line AB
    glm::vec3 c2 = ClosestPoint(Line(t.b, t.c), closest); // Line BC
    glm::vec3 c3 = ClosestPoint(Line(t.c, t.a), closest); // Line CA

    float magSq1 = MagnitudeSq(closest - c1);
    float magSq2 = MagnitudeSq(closest - c2);
    float magSq3 = MagnitudeSq(closest - c3);

    if (magSq1 < magSq2 && magSq1 < magSq3)
    {
        return c1;
    }
    else if (magSq2 < magSq1 && magSq2 < magSq3)
    {
        return c2;
    }

    return c3;
}
glm::vec3 ClosestPoint(const Ray &ray, const glm::vec3 &point)
{
    // Project point onto ray,
    float t = glm::dot(point - ray.origin, ray.direction);
    // Not needed if direction is normalized!
    // t /= Dot(ray.direction, ray.direction);

    // We only want to clamp t in the positive direction.
    // The ray extends infinatley in this direction!
    t = fmaxf(t, 0.0f);

    // Compute the projected position from the clamped t
    // Notice we multiply r.Normal by t, not AB.
    // This is becuase we want the ray in the direction
    // of the normal, which technically the line segment is
    // but this is much more explicit and easy to read.
    return glm::vec3(ray.origin + ray.direction * t);
}

glm::vec3 ClosestPoint(const Line &line, const glm::vec3 &point)
{
    glm::vec3 lVec = line.end - line.start; // Line Vector
    // Project "point" onto the "Line Vector", computing:
    // closest(t) = start + t * (end - start)
    // T is how far along the line the projected point is
    float t = glm::dot(point - line.start, lVec) / glm::dot(lVec, lVec);
    // Clamp t to the 0 to 1 range
    t = fmaxf(t, 0.0f);
    t = fminf(t, 1.0f);
    // Return projected position of t
    return line.start + lVec * t;
}

glm::vec3 ClosestPoint(const Plane &plane, const glm::vec3 &point)
{
    // This works assuming plane.Normal is normalized, which it should be
    float distance = glm::dot(plane.normal, point) - plane.distance;
    // If the plane normal wasn't normalized, we'd need this:
    // distance = distance / DOT(plane.Normal, plane.Normal);

    return point - plane.normal * distance;
}

glm::vec3 ClosestPoint(const OBB &obb, const glm::vec3 &point)
{
    glm::vec3 result = obb.position;
    glm::vec3 dir = point - obb.position;

    for (int i = 0; i < 3; ++i)
    {
        glm::vec3 orientation = obb.orientation[i * 3];
        glm::vec3 axis(orientation[0], orientation[1], orientation[2]);

        float distance = glm::dot(dir, axis);

        if (distance > obb.size[i])
        {
            distance = obb.size[i];
        }
        if (distance < -obb.size[i])
        {
            distance = -obb.size[i];
        }

        result = result + (axis * distance);
    }

    return result;
}

glm::vec3 ClosestPoint(const Sphere &sphere, const glm::vec3 &point)
{
    glm::vec3 sphereToPoint = point - sphere.position;
    sphereToPoint = glm::normalize(sphereToPoint);
    sphereToPoint = sphereToPoint * sphere.radius;
    return sphereToPoint + sphere.position;
}

glm::vec3 ClosestPoint(const AABB &aabb, const glm::vec3 &point)
{
    glm::vec3 result = point;
    glm::vec3 min = GetMin(aabb);
    glm::vec3 max = GetMax(aabb);

    result.x = (result.x < min.x) ? min.x : result.x;
    result.y = (result.y < min.x) ? min.y : result.y;
    result.z = (result.z < min.x) ? min.z : result.z;

    result.x = (result.x > max.x) ? max.x : result.x;
    result.y = (result.y > max.x) ? max.y : result.y;
    result.z = (result.z > max.x) ? max.z : result.z;

    return result;
}

glm::vec3 ClosestPoint(const glm::vec3 &point, const Sphere &sphere)
{
    return ClosestPoint(sphere, point);
}
glm::vec3 ClosestPoint(const glm::vec3 &point, const AABB &aabb)
{
    return ClosestPoint(aabb, point);
}
glm::vec3 ClosestPoint(const glm::vec3 &point, const OBB &obb)
{
    return ClosestPoint(obb, point);
}
glm::vec3 ClosestPoint(const glm::vec3 &point, const Plane &plane)
{
    return ClosestPoint(plane, point);
}
glm::vec3 ClosestPoint(const glm::vec3 &point, const Line &line)
{
    return ClosestPoint(line, point);
}
glm::vec3 ClosestPoint(const glm::vec3 &point, const Ray &ray)
{
    return ClosestPoint(ray, point);
}
glm::vec3 ClosestPoint(const glm::vec3 &p, const Triangle &t)
{
    return ClosestPoint(t, p);
}