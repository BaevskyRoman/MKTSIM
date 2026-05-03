#include "Model/Engine.hpp"
#include "Model/CollisionLogic.hpp"
#include "Config/VisualConfig.hpp"
#include "Utils/Utils.hpp"
#include <cmath>
#include <algorithm>
#include <limits>


namespace Model {

void Engine::spawnMoleculesInArea(const sf::FloatRect& area, int count, 
                        float min_speed, float max_speed, float mass, float radius) {
    for (int i = 0; i < count; ++i) {
        float px = Utils::Random::getFloat(area.position.x + radius, area.position.x + area.size.x - radius);
        float py = Utils::Random::getFloat(area.position.y + radius, area.position.y + area.size.y - radius);
        float angle = Utils::Random::getFloat(0.0f, 2.0f * Utils::PI);
        float speed = Utils::Random::getFloat(min_speed, max_speed);
        
        float vx = std::cos(angle) * speed;
        float vy = std::sin(angle) * speed;

        molecules_.push_back({sf::Vector2f(px, py), sf::Vector2f(vx, vy), mass, radius});
    }
}


void Engine::spawnMoleculesInArea(const sf::FloatRect& area, float concentration, float min_speed, float max_speed, 
                                float mass, float radius) {
    float areaSize = (area.size.x - 2*radius) * (area.size.y - 2*radius);
    float maxMolecules = areaSize / (4.0f * radius * radius);
    int count = static_cast<int>(maxMolecules * concentration);
    spawnMoleculesInArea(area, count, min_speed, max_speed, mass, radius);
}


void Engine::update(float deltaTime) {
    // --- GRAVITY ---
    if (enableGravity) {
        for (auto& mol : molecules_) {
            mol.velocity.y += gravityAcceleration * deltaTime;
        }
        for (auto& body : dynamicBodies_) {
            body.velocity.y += gravityAcceleration * deltaTime;
        }
    }

    // --- MOVE ---
    for (auto& mol : molecules_) {
        mol.move(deltaTime);
    }
    for (auto& body : dynamicBodies_) {
        body.move(deltaTime);
    }

    detectCollisions();
    resolveCollisions();
}


void Engine::detectCollisions() {
    eventsSD.clear();
    eventsDD.clear();
    eventsSM.clear();
    eventsDM.clear();
    eventsMM.clear();

    if (useGrid) {
        // --- MOL TO MOL ---
        grid.update(molecules_);
        grid.checkGrid(molecules_, eventsMM);
    } 
    else {
        // --- MOL TO MOL ---
        for (size_t i = 0; i < molecules_.size(); ++i) {
            for (size_t j = i + 1; j < molecules_.size(); ++j) {
                if (Geo::isColliding(molecules_[i], molecules_[j])) eventsMM.emplace_back(i, j);
            }
        }
    }

    // --- STATIC BODY TO DYNAMIC BODY ---
    for (size_t s = 0; s < staticBodies_.size(); ++s) {
        for (size_t d = 0; d < dynamicBodies_.size(); ++d) {
            if (Geo::isColliding(staticBodies_[s], dynamicBodies_[d])) eventsSD.emplace_back(s, d);
        }
    }

    // --- DYNAMIC BODY TO DYNAMIC BODY ---
    for (size_t i = 0; i < dynamicBodies_.size(); ++i) {
        for (size_t j = i + 1; j < dynamicBodies_.size(); ++j) {
            if (Geo::isColliding(dynamicBodies_[i], dynamicBodies_[j])) eventsDD.emplace_back(i, j);
        }
    }

    // --- STATIC BODY TO MOL ---
    for (size_t s = 0; s < staticBodies_.size(); ++s) {
        for (size_t m = 0; m < molecules_.size(); ++m) {
            if (Geo::isColliding(staticBodies_[s], molecules_[m])) eventsSM.emplace_back(s, m);
        }
    }

    // --- DYNAMIC BODY TO MOL ---
    for (size_t d = 0; d < dynamicBodies_.size(); ++d) {
        for (size_t m = 0; m < molecules_.size(); ++m) {
            if (Geo::isColliding(dynamicBodies_[d], molecules_[m])) eventsDM.emplace_back(d, m);
        }
    }

    Utils::Random::shuffle(eventsDD);
    Utils::Random::shuffle(eventsDM);
    Utils::Random::shuffle(eventsMM);
}


void Engine::resolveCollisions() {
    for (auto& event : eventsMM) {
        resolveCollision(event);
    }

    for (auto& event : eventsDM) {
        resolveCollision(event);
    }

    for (auto& event : eventsSM) {
        resolveCollision(event);
    }

    for (auto& event : eventsDD) {
        resolveCollision(event);
    }
    
    for (auto& event : eventsSD) {
        resolveCollision(event);
    }
}


void Engine::resolveCollision(EventSD& event) {
    const StaticBody& sBody = staticBodies_[event.indexA];
    DynamicBody& dBody = dynamicBodies_[event.indexB];
    
    float cosD = std::cos(dBody.angle), sinD = std::sin(dBody.angle);
    float cosS = std::cos(sBody.angle), sinS = std::sin(sBody.angle);
    
    sf::Vector2f axes[4] = {
        {cosD, sinD},   // Локальная ось X динамического тела
        {-sinD, cosD},  // Локальная ось Y динамического тела
        {cosS, sinS},   // Локальная ось X статического тела
        {-sinS, cosS}   // Локальная ось Y статического тела
    };
    
    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f collisionNormal;
    
    for (int i = 0; i < 4; ++i) {
        float min1, max1, min2, max2;
        Utils::project(dBody.vertices.data(), axes[i], min1, max1);
        Utils::project(sBody.vertices.data(), axes[i], min2, max2);
        
        float overlap = std::min(max1, max2) - std::max(min1, min2);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            collisionNormal = axes[i];
        }
    }
    
    sf::Vector2f direction = dBody.position - sBody.position;
    if (Utils::dot(direction, collisionNormal) < 0) {
        collisionNormal = -collisionNormal;
    }

    sf::Vector2f contactPoint = dBody.vertices[0];
    float minProj = Utils::dot(dBody.vertices[0], collisionNormal);
    for (int i = 1; i < 4; ++i) {
        float proj = Utils::dot(dBody.vertices[i], collisionNormal);
        if (proj < minProj) {
            minProj = proj;
            contactPoint = dBody.vertices[i];
        }
    }

    sf::Vector2f rBody = contactPoint - dBody.position;
    sf::Vector2f vBodyPoint = dBody.velocity + Utils::cross(dBody.angularVelocity, rBody);
    
    float velAlongNormal = Utils::dot(vBodyPoint, collisionNormal);
    
    if (velAlongNormal < 0) {
        float rBodyCrossN = Utils::cross(rBody, collisionNormal);
        
        float invMassBody = 1.0f / dBody.mass;
        float invInertiaBody = 1.0f / dBody.inertia;
        
        float j = -2.0f * velAlongNormal;
        j /= (invMassBody + (rBodyCrossN * rBodyCrossN) * invInertiaBody);
        
        sf::Vector2f impulse = collisionNormal * j;
        
        dBody.velocity += impulse * invMassBody;
        dBody.angularVelocity += rBodyCrossN * j * invInertiaBody;
    }
}


void Engine::resolveCollision(EventDD& event) {
    DynamicBody& bodyA = dynamicBodies_[event.indexA];
    DynamicBody& bodyB = dynamicBodies_[event.indexB];

    float cosA = std::cos(bodyA.angle);
    float sinA = std::sin(bodyA.angle);
    float cosB = std::cos(bodyB.angle);
    float sinB = std::sin(bodyB.angle);
    
    sf::Vector2f axes[4] = {
        {cosA, sinA}, {-sinA, cosA},
        {cosB, sinB}, {-sinB, cosB}
    };

    float minOverlap = std::numeric_limits<float>::max();
    sf::Vector2f collisionNormal;

    // 1. Поиск нормали коллизии (ось с наименьшим перекрытием)
    for (int k = 0; k < 4; ++k) {
        float min1, max1, min2, max2;
        Utils::project(bodyA.vertices.data(), axes[k], min1, max1);
        Utils::project(bodyB.vertices.data(), axes[k], min2, max2);

        float overlap = std::min(max1, max2) - std::max(min1, min2);
        if (overlap < minOverlap) {
            minOverlap = overlap;
            collisionNormal = axes[k];
        }
    }

    // 2. Убеждаемся, что нормаль направлена от A к B
    sf::Vector2f direction = bodyB.position - bodyA.position;
    if (Utils::dot(direction, collisionNormal) < 0) {
        collisionNormal = -collisionNormal;
    }

    // (Позиционная коррекция и повторный getVertices удалены)

    // 3. Поиск Точки Контакта (используем УЖЕ рассчитанные vertsA и vertsB)
    sf::Vector2f contactA = bodyA.vertices[0];
    float maxProjA = Utils::dot(bodyA.vertices[0], collisionNormal);
    for (int k = 1; k < 4; ++k) {
        float proj = Utils::dot(bodyA.vertices[k], collisionNormal);
        if (proj > maxProjA) {
            maxProjA = proj;
            contactA = bodyA.vertices[k];
        }
    }

    sf::Vector2f contactB = bodyB.vertices[0];
    float maxProjB = Utils::dot(bodyB.vertices[0], -collisionNormal);
    for (int k = 1; k < 4; ++k) {
        float proj = Utils::dot(bodyB.vertices[k], -collisionNormal);
        if (proj > maxProjB) {
            maxProjB = proj;
            contactB = bodyB.vertices[k];
        }
    }

    sf::Vector2f contactPoint = (contactA + contactB) * 0.5f;

    // 4. Разрешение Импульса
    sf::Vector2f rA = contactPoint - bodyA.position;
    sf::Vector2f rB = contactPoint - bodyB.position;

    sf::Vector2f vA_point = bodyA.velocity + Utils::cross(bodyA.angularVelocity, rA);
    sf::Vector2f vB_point = bodyB.velocity + Utils::cross(bodyB.angularVelocity, rB);

    sf::Vector2f relativeVelocity = vB_point - vA_point;
    float velAlongNormal = Utils::dot(relativeVelocity, collisionNormal);

    // Применяем импульс только если тела двигаются навстречу друг другу
    if (velAlongNormal < 0) {
        float e = 1.0f; 
        
        float rAcrossN = Utils::cross(rA, collisionNormal);
        float rBcrossN = Utils::cross(rB, collisionNormal);

        float invInertiaA = 1.0f / bodyA.inertia;
        float invInertiaB = 1.0f / bodyB.inertia;
        float invMassA = 1.0f / bodyA.mass;
        float invMassB = 1.0f / bodyB.mass;

        float j = -(1.0f + e) * velAlongNormal;
        j /= (invMassA + invMassB + 
                (rAcrossN * rAcrossN) * invInertiaA + 
                (rBcrossN * rBcrossN) * invInertiaB);

        sf::Vector2f impulse = collisionNormal * j;

        bodyA.velocity -= impulse * invMassA;
        bodyA.angularVelocity -= rAcrossN * j * invInertiaA;

        bodyB.velocity += impulse * invMassB;
        bodyB.angularVelocity += rBcrossN * j * invInertiaB;
    }
}


void Engine::resolveCollision(EventSM& event) {
    const StaticBody& body = staticBodies_[event.indexA];
    Molecule& mol = molecules_[event.indexB];

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
    
    float distance = std::sqrt(distX * distX + distY * distY);
    
    sf::Vector2f localNormal;

    if (distance > 0.0f) {
        localNormal = sf::Vector2f(distX / distance, distY / distance);
    } else {
        float distToLeft = localX - (-halfW);
        float distToRight = halfW - localX;
        float distToBottom = localY - (-halfH);
        float distToTop = halfH - localY;

        float minDist = std::min({distToLeft, distToRight, distToBottom, distToTop});

        if (minDist == distToLeft) localNormal = {-1.0f, 0.0f};
        else if (minDist == distToRight) localNormal = {1.0f, 0.0f};
        else if (minDist == distToBottom) localNormal = {0.0f, -1.0f};
        else localNormal = {0.0f, 1.0f};
    }

    sf::Vector2f worldNormal(
        localNormal.x * cosA - localNormal.y * sinA,
        localNormal.x * sinA + localNormal.y * cosA
    );

    float velocityAlongNormal = Utils::dot(mol.velocity, worldNormal);
    
    if (velocityAlongNormal < 0) {
        mol.velocity -= worldNormal * (2.0f * velocityAlongNormal);
    }
}


void Engine::resolveCollision(EventDM& event) {
    DynamicBody& body = dynamicBodies_[event.indexA];
    Molecule& mol = molecules_[event.indexB];

    // Заново вычисляем локальные координаты, так как они нужны для нормали и точки контакта
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
    
    float distance = std::sqrt(distX * distX + distY * distY);
    
    sf::Vector2f localNormal;

    // Определение локальной нормали
    if (distance > 0.0f) {
        localNormal = sf::Vector2f(distX / distance, distY / distance);
    } else {
        // Редкий случай: центр молекулы внутри тела
        float distToLeft = localX - (-halfW);
        float distToRight = halfW - localX;
        float distToBottom = localY - (-halfH);
        float distToTop = halfH - localY;

        float minDist = std::min({distToLeft, distToRight, distToBottom, distToTop});

        if (minDist == distToLeft) { 
            localNormal = {-1.0f, 0.0f}; closestLocalX = -halfW; 
        } else if (minDist == distToRight) { 
            localNormal = {1.0f, 0.0f}; closestLocalX = halfW; 
        } else if (minDist == distToBottom) { 
            localNormal = {0.0f, -1.0f}; closestLocalY = -halfH; 
        } else { 
            localNormal = {0.0f, 1.0f}; closestLocalY = halfH; 
        }
    }

    // Перевод нормали обратно в мировые координаты
    sf::Vector2f worldNormal(
        localNormal.x * cosA - localNormal.y * sinA,
        localNormal.x * sinA + localNormal.y * cosA
    );

    // Перевод точки контакта обратно в мировые координаты
    sf::Vector2f worldContactPoint(
        closestLocalX * cosA - closestLocalY * sinA + body.position.x,
        closestLocalX * sinA + closestLocalY * cosA + body.position.y
    );

    // Подготовка масс и инерции
    float invMassMol = 1.0f / mol.mass;
    float invMassBody = 1.0f / body.mass;
    float invInertiaBody = 1.0f / body.inertia;

    // Вектор от центра масс тела к точке контакта
    sf::Vector2f rBody = worldContactPoint - body.position;
    
    // Линейные скорости в точке контакта
    sf::Vector2f vBodyPoint = body.velocity + Utils::cross(body.angularVelocity, rBody);
    sf::Vector2f vMolPoint = mol.velocity; // Молекула не вращается, берем центр

    sf::Vector2f relativeVelocity = vMolPoint - vBodyPoint;
    float velAlongNormal = Utils::dot(relativeVelocity, worldNormal);

    // Применяем импульс, только если объекты сближаются
    if (velAlongNormal < 0) {
        float e = 1.0f; // Абсолютно упругий удар
        float rBodyCrossN = Utils::cross(rBody, worldNormal);

        float j = -(1.0f + e) * velAlongNormal;
        j /= (invMassMol + invMassBody + (rBodyCrossN * rBodyCrossN) * invInertiaBody);

        sf::Vector2f impulse = worldNormal * j;

        mol.velocity += impulse * invMassMol;
        body.velocity -= impulse * invMassBody;
        body.angularVelocity -= rBodyCrossN * j * invInertiaBody;
    }
}


void Engine::resolveCollision(EventMM& event) {
    Molecule& molA = molecules_[event.indexA];
    Molecule& molB = molecules_[event.indexB];

    sf::Vector2f delta = molB.position - molA.position;
    float distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    
    sf::Vector2f normal;
    if (Utils::isZero(distance)) {
        float angle = Utils::Random::getFloat(0.0f, 2.0f * Utils::PI);
        normal = {std::cos(angle), std::sin(angle)};
    }
    else {
        normal = delta / distance;
    }

    sf::Vector2f relativeVelocity = molA.velocity - molB.velocity;
    float velocityAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

    if (velocityAlongNormal < 0) {
        return;
    }

    float impulseScalar = (2.0f * velocityAlongNormal) / (molA.mass + molB.mass);
    molA.velocity -= normal * (impulseScalar * molB.mass);
    molB.velocity += normal * (impulseScalar * molA.mass);
}


}