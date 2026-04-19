#include <cmath>
#include <algorithm>
#include "Model/Engine.hpp"
#include "Config/VisualConfig.hpp"
#include "Utils/Math.hpp"
#include <limits>

namespace {
    void getVertices(const Model::DynamicBody& body, sf::Vector2f out[4]) {
        float hw = body.size.x / 2.0f;
        float hh = body.size.y / 2.0f;
        float cosA = std::cos(body.angle);
        float sinA = std::sin(body.angle);
        
        sf::Vector2f corners[4] = {{-hw, -hh}, {hw, -hh}, {hw, hh}, {-hw, hh}};
        for (int i = 0; i < 4; ++i) {
            out[i] = sf::Vector2f(
                corners[i].x * cosA - corners[i].y * sinA + body.position.x,
                corners[i].x * sinA + corners[i].y * cosA + body.position.y
            );
        }
    }

    void getVertices(const sf::FloatRect& rect, sf::Vector2f out[4]) {
        out[0] = sf::Vector2f(rect.position.x, rect.position.y);
        out[1] = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y);
        out[2] = sf::Vector2f(rect.position.x + rect.size.x, rect.position.y + rect.size.y);
        out[3] = sf::Vector2f(rect.position.x, rect.position.y + rect.size.y);
    }

    void project(const sf::Vector2f vertices[4], const sf::Vector2f& axis, float& min, float& max) {
        min = max = Utils::Math::dot(vertices[0], axis);
        for (int i = 1; i < 4; ++i) {
            float proj = Utils::Math::dot(vertices[i], axis);
            if (proj < min) min = proj;
            if (proj > max) max = proj;
        }
    }
}

namespace Model {

const std::vector<Molecule>& Engine::getMolecules() const { return molecules_; }
const std::vector<sf::FloatRect>& Engine::getStaticBodies() const { return staticBodies_; }
void Engine::spawnStaticBody(const sf::FloatRect& rect) { staticBodies_.push_back(rect); }

const std::vector<DynamicBody>& Engine::getDynamicBodies() const { return dynamicBodies_; }
void Engine::spawnDynamicBody(sf::Vector2f sz, sf::Vector2f pos, float m) { 
    dynamicBodies_.push_back(DynamicBody(sz, pos, m)); 
}


void Engine::update(float deltaTime) {
    // --- GRAVITY ---
    if (Config::Physics::GRAVITY) {
        for (auto& mol : molecules_) {
            mol.velocity.y += Config::Physics::GRAVITY_ACCELERATION * deltaTime;
        }
        for (auto& body : dynamicBodies_) {
            body.velocity.y += Config::Physics::GRAVITY_ACCELERATION * deltaTime;
        }
    }

    // --- MOVE ---
    for (auto& mol : molecules_) {
        mol.move(deltaTime);
    }
    for (auto& body : dynamicBodies_) {
        body.move(deltaTime);
    }

    handleCollision();
}


void Engine::handleCollision() {
    // --- MOL TO STATIC BODY ---
    for (auto& mol : molecules_) {
        for (const auto& body : staticBodies_) {
            handleCollision(mol, body);
        }
    }

    // --- MOL TO DYNAMIC BODY ---
    for (auto& mol : molecules_) {
        for (auto& body : dynamicBodies_) {
            handleCollision(mol, body);
        }
    }

    // --- DYNAMIC BODY TO STATIC BODY ---
    for (auto& dBody : dynamicBodies_) {
        for (const auto& sBody : staticBodies_) {
            handleCollision(dBody, sBody);
        }
    }

    // --- DYNAMIC BODY TO DYNAMIC BODY ---
    for (size_t i = 0; i < dynamicBodies_.size(); ++i) {
        for (size_t j = i + 1; j < dynamicBodies_.size(); ++j) {
            handleCollision(dynamicBodies_[i], dynamicBodies_[j]);
        }
    }


    // --- MOL TO MOL ---
    for (size_t i = 0; i < molecules_.size(); ++i) {
        for (size_t j = i + 1; j < molecules_.size(); ++j) {
            handleCollision(molecules_[i], molecules_[j]);
        }
    }
}


void Engine::handleCollision(Molecule& mol, const sf::FloatRect& body) {
    float closestX = std::clamp(mol.position.x, body.position.x, body.position.x + body.size.x);
    float closestY = std::clamp(mol.position.y, body.position.y, body.position.y + body.size.y);

    float deltaX = mol.position.x - closestX;
    float deltaY = mol.position.y - closestY;

    float distancePow2 = deltaX * deltaX + deltaY * deltaY;
    if (distancePow2 < mol.radius * mol.radius) {
        float distance = std::sqrt(distancePow2);
        sf::Vector2f normal;
        float penetration = 0.0f;

        if (distance > 0.0f) {
            normal = sf::Vector2f(deltaX / distance, deltaY / distance);
            penetration = mol.radius - distance;
        } else {
            float distToLeft = mol.position.x - body.position.x;
            float distToRight = (body.position.x + body.size.x) - mol.position.x;
            float distToTop = mol.position.y - body.position.y;
            float distToBottom = (body.position.y + body.size.y) - mol.position.y;

            float minDist = std::min({distToLeft, distToRight, distToTop, distToBottom});

            if (minDist == distToLeft) { normal = {-1.0f, 0.0f}; penetration = distToLeft + mol.radius; }
            else if (minDist == distToRight) { normal = {1.0f, 0.0f}; penetration = distToRight + mol.radius; }
            else if (minDist == distToTop) { normal = {0.0f, -1.0f}; penetration = distToTop + mol.radius; }
            else { normal = {0.0f, 1.0f}; penetration = distToBottom + mol.radius; }
        }

        float velocityAlongNormal = mol.velocity.x * normal.x + mol.velocity.y * normal.y;
        
        if (velocityAlongNormal < 0) {
            mol.velocity -= normal * (2.0f * velocityAlongNormal);
        }
    }
}


void Engine::handleCollision(Molecule& mol, DynamicBody& body) {
    float dx = mol.position.x - body.position.x;
    float dy = mol.position.y - body.position.y;

    float cosA = std::cos(body.angle);
    float sinA = std::sin(body.angle);

    float localX = dx * cosA + dy * sinA;
    float localY = -dx * sinA + dy * cosA;

    float halfW = body.size.x / 2.0f;
    float halfH = body.size.y / 2.0f;

    float closestLocalX = std::clamp(localX, -halfW, halfW);
    float closestLocalY = std::clamp(localY, -halfH, halfH);

    float distX = localX - closestLocalX;
    float distY = localY - closestLocalY;
    float distancePow2 = distX * distX + distY * distY;

    if (distancePow2 < mol.radius * mol.radius) {
        float distance = std::sqrt(distancePow2);
        
        sf::Vector2f localNormal;
        float penetration = 0.0f;

        if (distance > 0.0f) {
            localNormal = sf::Vector2f(distX / distance, distY / distance);
            penetration = mol.radius - distance;
        } else {
            float distToLeft = localX - (-halfW);
            float distToRight = halfW - localX;
            float distToBottom = localY - (-halfH);
            float distToTop = halfH - localY;

            float minDist = std::min({distToLeft, distToRight, distToBottom, distToTop});

            if (minDist == distToLeft) { 
                localNormal = {-1.0f, 0.0f}; penetration = distToLeft + mol.radius; closestLocalX = -halfW; 
            } else if (minDist == distToRight) { 
                localNormal = {1.0f, 0.0f}; penetration = distToRight + mol.radius; closestLocalX = halfW; 
            } else if (minDist == distToBottom) { 
                localNormal = {0.0f, -1.0f}; penetration = distToBottom + mol.radius; closestLocalY = -halfH; 
            } else { 
                localNormal = {0.0f, 1.0f}; penetration = distToTop + mol.radius; closestLocalY = halfH; 
            }
        }

        sf::Vector2f worldNormal(
            localNormal.x * cosA - localNormal.y * sinA,
            localNormal.x * sinA + localNormal.y * cosA
        );

        sf::Vector2f worldContactPoint(
            closestLocalX * cosA - closestLocalY * sinA + body.position.x,
            closestLocalX * sinA + closestLocalY * cosA + body.position.y
        );

        float invMassMol = 1.0f / mol.mass;
        float invMassBody = 1.0f / body.mass;
        float invMassTotal = invMassMol + invMassBody;

        sf::Vector2f rBody = worldContactPoint - body.position;
        
        sf::Vector2f vBodyPoint = body.velocity + Utils::Math::cross(body.angularVelocity, rBody);
        sf::Vector2f vMolPoint = mol.velocity;

        sf::Vector2f relativeVelocity = vMolPoint - vBodyPoint;
        float velAlongNormal = Utils::Math::dot(relativeVelocity, worldNormal);

        if (velAlongNormal < 0) {
            float e = 1.0f;
            float rBodyCrossN = Utils::Math::cross(rBody, worldNormal);
            float invInertiaBody = 1.0f / body.inertia;

            float j = -(1.0f + e) * velAlongNormal;
            j /= (invMassMol + invMassBody + (rBodyCrossN * rBodyCrossN) * invInertiaBody);

            sf::Vector2f impulse = worldNormal * j;

            mol.velocity += impulse * invMassMol;
            body.velocity -= impulse * invMassBody;
            body.angularVelocity -= rBodyCrossN * j * invInertiaBody;
        }
    }
}


void Engine::handleCollision(DynamicBody& dBody, const sf::FloatRect& sBody) {
    sf::Vector2f dynVerts[4];
    getVertices(dBody, dynVerts);
    
    sf::Vector2f statVerts[4];
    getVertices(sBody, statVerts);
    
    float cosA = std::cos(dBody.angle);
    float sinA = std::sin(dBody.angle);
    sf::Vector2f axes[4] = {
        {1.0f, 0.0f}, {0.0f, 1.0f},
        {cosA, sinA}, {-sinA, cosA}
    };
    
    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f collisionNormal;
    bool isColliding = true;
    
    for (int i = 0; i < 4; ++i) {
        float min1, max1, min2, max2;
        project(dynVerts, axes[i], min1, max1);
        project(statVerts, axes[i], min2, max2);
        
        if (max1 < min2 || max2 < min1) {
            isColliding = false; 
            break;
        }
        
        float overlap = std::min(max1, max2) - std::max(min1, min2);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            collisionNormal = axes[i];
        }
    }
    
    if (isColliding) {
        sf::Vector2f staticCenter(
            sBody.position.x + sBody.size.x / 2.0f, 
            sBody.position.y + sBody.size.y / 2.0f
        );
        sf::Vector2f direction = dBody.position - staticCenter;
        if (Utils::Math::dot(direction, collisionNormal) < 0) {
            collisionNormal = -collisionNormal;
        }

        dBody.position += collisionNormal * minOverlap;

        getVertices(dBody, dynVerts);

        sf::Vector2f contactPoint = dynVerts[0];
        float minProj = Utils::Math::dot(dynVerts[0], collisionNormal);
        for (int i = 1; i < 4; ++i) {
            float proj = Utils::Math::dot(dynVerts[i], collisionNormal);
            if (proj < minProj) {
                minProj = proj;
                contactPoint = dynVerts[i];
            }
        }

        sf::Vector2f rBody = contactPoint - dBody.position;
        
        sf::Vector2f vBodyPoint = dBody.velocity + Utils::Math::cross(dBody.angularVelocity, rBody);
        
        float velAlongNormal = Utils::Math::dot(vBodyPoint, collisionNormal);
        
        if (velAlongNormal < 0) {
            float e = 1.f;
            float rBodyCrossN = Utils::Math::cross(rBody, collisionNormal);
            
            float invMassBody = 1.0f / dBody.mass;
            float invInertiaBody = 1.0f / dBody.inertia;
            
            float j = -(1.0f + e) * velAlongNormal;
            j /= (invMassBody + (rBodyCrossN * rBodyCrossN) * invInertiaBody);
            
            sf::Vector2f impulse = collisionNormal * j;
            
            dBody.velocity += impulse * invMassBody;
            dBody.angularVelocity += rBodyCrossN * j * invInertiaBody;
        }
    }
}


void Engine::handleCollision(DynamicBody& bodyA, DynamicBody& bodyB) {
    sf::Vector2f vertsA[4];
    getVertices(bodyA, vertsA);
    
    sf::Vector2f vertsB[4];
    getVertices(bodyB, vertsB);

    // Получаем 4 оси для проверки (2 от тела A, 2 от тела B)
    float cosA = std::cos(bodyA.angle);
    float sinA = std::sin(bodyA.angle);
    float cosB = std::cos(bodyB.angle);
    float sinB = std::sin(bodyB.angle);
    
    sf::Vector2f axes[4] = {
        {cosA, sinA}, {-sinA, cosA}, // Локальные оси тела A
        {cosB, sinB}, {-sinB, cosB}  // Локальные оси тела B
    };

    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f collisionNormal;
    bool isColliding = true;

    // 1. Поиск пересечений по алгоритму SAT
    for (int k = 0; k < 4; ++k) {
        float min1, max1, min2, max2;
        project(vertsA, axes[k], min1, max1);
        project(vertsB, axes[k], min2, max2);

        if (max1 < min2 || max2 < min1) {
            isColliding = false; // Нашли разделяющую ось, столкновения нет
            break;
        }

        float overlap = std::min(max1, max2) - std::max(min1, min2);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            collisionNormal = axes[k];
        }
    }

    if (isColliding) {
        // 2. Убеждаемся, что нормаль направлена от A к B
        sf::Vector2f direction = bodyB.position - bodyA.position;
        if (Utils::Math::dot(direction, collisionNormal) < 0) {
            collisionNormal = -collisionNormal;
        }

        // 3. Позиционная коррекция (раздвигаем оба тела)
        float invMassA = 1.0f / bodyA.mass;
        float invMassB = 1.0f / bodyB.mass;
        float invMassTotal = invMassA + invMassB;

        bodyA.position -= collisionNormal * (minOverlap * (invMassA / invMassTotal));
        bodyB.position += collisionNormal * (minOverlap * (invMassB / invMassTotal));

        // Пересчитываем вершины после коррекции позиции для точного поиска точки контакта
        getVertices(bodyA, vertsA);
        getVertices(bodyB, vertsB);

        // 4. Поиск Точки Контакта (Эвристика для OBB)
        // Ищем вершину тела A, которая глубже всего проникла по направлению нормали
        sf::Vector2f contactA = vertsA[0];
        float maxProjA = Utils::Math::dot(vertsA[0], collisionNormal);
        for (int k = 1; k < 4; ++k) {
            float proj = Utils::Math::dot(vertsA[k], collisionNormal);
            if (proj > maxProjA) {
                maxProjA = proj;
                contactA = vertsA[k];
            }
        }

        // Ищем вершину тела B, которая глубже всего проникла ПРОТИВ нормали
        sf::Vector2f contactB = vertsB[0];
        float maxProjB = Utils::Math::dot(vertsB[0], -collisionNormal);
        for (int k = 1; k < 4; ++k) {
            float proj = Utils::Math::dot(vertsB[k], -collisionNormal);
            if (proj > maxProjB) {
                maxProjB = proj;
                contactB = vertsB[k];
            }
        }

        // Усредняем обе "глубокие" вершины для получения стабильной точки контакта
        sf::Vector2f contactPoint = (contactA + contactB) * 0.5f;

        // 5. Разрешение Импульса
        sf::Vector2f rA = contactPoint - bodyA.position;
        sf::Vector2f rB = contactPoint - bodyB.position;

        // Линейные скорости точек контакта с учетом вращения
        sf::Vector2f vA_point = bodyA.velocity + Utils::Math::cross(bodyA.angularVelocity, rA);
        sf::Vector2f vB_point = bodyB.velocity + Utils::Math::cross(bodyB.angularVelocity, rB);

        // Относительная скорость (насколько быстро точка B движется относительно точки A)
        sf::Vector2f relativeVelocity = vB_point - vA_point;
        float velAlongNormal = Utils::Math::dot(relativeVelocity, collisionNormal);

        // Если тела двигаются навстречу друг другу
        if (velAlongNormal < 0) {
            float e = 1.0f; // Абсолютно упругий отскок
            
            float rAcrossN = Utils::Math::cross(rA, collisionNormal);
            float rBcrossN = Utils::Math::cross(rB, collisionNormal);

            float invInertiaA = 1.0f / bodyA.inertia;
            float invInertiaB = 1.0f / bodyB.inertia;

            // Полная формула импульса для двух вращающихся тел
            float j = -(1.0f + e) * velAlongNormal;
            j /= (invMassA + invMassB + 
                    (rAcrossN * rAcrossN) * invInertiaA + 
                    (rBcrossN * rBcrossN) * invInertiaB);

            sf::Vector2f impulse = collisionNormal * j;

            // Применяем импульс к телу A (знак минус, т.к. нормаль направлена от него)
            bodyA.velocity -= impulse * invMassA;
            bodyA.angularVelocity -= rAcrossN * j * invInertiaA;

            // Применяем импульс к телу B
            bodyB.velocity += impulse * invMassB;
            bodyB.angularVelocity += rBcrossN * j * invInertiaB;
        }
    }
}


void Engine::handleCollision(Molecule& m1, Molecule& m2) {
    sf::Vector2f delta = m2.position - m1.position;

    float deltaPow2 = delta.x * delta.x + delta.y * delta.y;
    float minDist = m1.radius + m2.radius;

    if (deltaPow2 < minDist * minDist) {
        float distance = std::sqrt(deltaPow2);
        
        sf::Vector2f normal;
        float overlap;
        if (Utils::Math::isZero(distance)) {
            float angle = Utils::Math::Random::getFloat(0.0f, 2.0f * Utils::Math::PI);
            normal = {std::cos(angle), std::sin(angle)};
            overlap = minDist;
        }
        else {
            normal = delta / distance;
            overlap = minDist - distance;
        }

        sf::Vector2f relativeVelocity = m1.velocity - m2.velocity;
        float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

        if (velocityAlongNormal < 0) {
            return;
        }

        float impulseScalar = (2.0f * velocityAlongNormal) / (m1.mass + m2.mass);
        m1.velocity -= normal * (impulseScalar * m2.mass);
        m2.velocity += normal * (impulseScalar * m1.mass);
    }
}


void Engine::spawnMoleculesInArea(const sf::FloatRect& area, float concentration, 
                        float min_speed, float max_speed, float mass, float radius) {
    float areaSize = (area.size.x - 2*radius) * (area.size.y - 2*radius);
    float maxMolecules = areaSize / (4.0f * radius * radius);
    int count = static_cast<int>(maxMolecules * concentration);
    
    for (int i = 0; i < count; ++i) {
        float px = Utils::Math::Random::getFloat(area.position.x + radius, area.position.x + area.size.x - radius);
        float py = Utils::Math::Random::getFloat(area.position.y + radius, area.position.y + area.size.y - radius);
        float angle = Utils::Math::Random::getFloat(0.0f, 2.0f * Utils::Math::PI);
        float speed = Utils::Math::Random::getFloat(min_speed, max_speed);
        
        float vx = std::cos(angle) * speed;
        float vy = std::sin(angle) * speed;

        molecules_.push_back({sf::Vector2f(px, py), sf::Vector2f(vx, vy), mass, radius});
    }
}


void Engine::spawnMolecule(float x, float y, float vx, float vy) {
    molecules_.push_back({sf::Vector2f(x, y), sf::Vector2f(vx, vy)});
}

}