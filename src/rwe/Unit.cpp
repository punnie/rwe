#include "Unit.h"
#include <rwe/GameScene.h>
#include <rwe/geometry/Plane3f.h>
#include <rwe/math/rwe_math.h>

namespace rwe
{
    MoveOrder::MoveOrder(const Vector3f& destination) : destination(destination)
    {
    }

    UnitOrder createMoveOrder(const Vector3f& destination)
    {
        return MoveOrder(destination);
    }

    UnitOrder createAttackOrder(UnitId target)
    {
        return AttackOrder(target);
    }

    UnitOrder createAttackGroundOrder(const Vector3f& target)
    {
        return AttackOrder(target);
    }

    float Unit::toRotation(const Vector3f& direction)
    {
        return Vector3f(0.0f, 0.0f, 1.0f).angleTo(direction, Vector3f(0.0f, 1.0f, 0.0f));
    }

    Vector3f Unit::toDirection(float rotation)
    {
        return Matrix4f::rotationY(rotation) * Vector3f(0.0f, 0.0f, 1.0f);
    }

    Unit::Unit(const UnitMesh& mesh, std::unique_ptr<CobEnvironment>&& cobEnvironment, SelectionMesh&& selectionMesh)
        : mesh(mesh), cobEnvironment(std::move(cobEnvironment)), selectionMesh(std::move(selectionMesh))
    {
    }

    void Unit::moveObject(const std::string& pieceName, Axis axis, float targetPosition, float speed)
    {
        auto piece = mesh.find(pieceName);
        if (!piece)
        {
            throw std::runtime_error("Invalid piece name: " + pieceName);
        }

        UnitMesh::MoveOperation op(targetPosition, speed);

        switch (axis)
        {
            case Axis::X:
                piece->get().xMoveOperation = op;
                break;
            case Axis::Y:
                piece->get().yMoveOperation = op;
                break;
            case Axis::Z:
                piece->get().zMoveOperation = op;
                break;
        }
    }

    void Unit::moveObjectNow(const std::string& pieceName, Axis axis, float targetPosition)
    {
        auto piece = mesh.find(pieceName);
        if (!piece)
        {
            throw std::runtime_error("Invalid piece name: " + pieceName);
        }

        switch (axis)
        {
            case Axis::X:
                piece->get().offset.x = targetPosition;
                piece->get().xMoveOperation = std::nullopt;
                break;
            case Axis::Y:
                piece->get().offset.y = targetPosition;
                piece->get().yMoveOperation = std::nullopt;
                break;
            case Axis::Z:
                piece->get().offset.z = targetPosition;
                piece->get().zMoveOperation = std::nullopt;
                break;
        }
    }

    void Unit::turnObject(const std::string& pieceName, Axis axis, RadiansAngle targetAngle, float speed)
    {
        auto piece = mesh.find(pieceName);
        if (!piece)
        {
            throw std::runtime_error("Invalid piece name: " + pieceName);
        }

        UnitMesh::TurnOperation op(targetAngle, toRadians(speed));

        switch (axis)
        {
            case Axis::X:
                piece->get().xTurnOperation = op;
                break;
            case Axis::Y:
                piece->get().yTurnOperation = op;
                break;
            case Axis::Z:
                piece->get().zTurnOperation = op;
                break;
        }
    }

    void Unit::turnObjectNow(const std::string& pieceName, Axis axis, RadiansAngle targetAngle)
    {
        auto piece = mesh.find(pieceName);
        if (!piece)
        {
            throw std::runtime_error("Invalid piece name: " + pieceName);
        }

        switch (axis)
        {
            case Axis::X:
                piece->get().rotation.x = targetAngle.value;
                piece->get().xTurnOperation = std::nullopt;
                break;
            case Axis::Y:
                piece->get().rotation.y = targetAngle.value;
                piece->get().yTurnOperation = std::nullopt;
                break;
            case Axis::Z:
                piece->get().rotation.z = targetAngle.value;
                piece->get().zTurnOperation = std::nullopt;
                break;
        }
    }

    void Unit::spinObject(const std::string& pieceName, Axis axis, float speed, float acceleration)
    {
        auto piece = mesh.find(pieceName);
        if (!piece)
        {
            throw std::runtime_error("Invalid piece name: " + pieceName);
        }

        UnitMesh::SpinOperation op(acceleration == 0.0f ? toRadians(speed) : 0.0f, toRadians(speed), toRadians(acceleration));

        switch (axis)
        {
            case Axis::X:
                piece->get().xTurnOperation = op;
                break;
            case Axis::Y:
                piece->get().yTurnOperation = op;
                break;
            case Axis::Z:
                piece->get().zTurnOperation = op;
                break;
        }
    }

    void setStopSpinOp(std::optional<UnitMesh::TurnOperationUnion>& existingOp, float deceleration)
    {
        if (!existingOp)
        {
            return;
        }
        auto spinOp = boost::get<UnitMesh::SpinOperation>(&*existingOp);
        if (spinOp == nullptr)
        {
            return;
        }

        if (deceleration == 0.0f)
        {
            existingOp = std::nullopt;
            return;
        }

        existingOp = UnitMesh::StopSpinOperation(spinOp->currentSpeed, toRadians(deceleration));
    }

    void Unit::stopSpinObject(const std::string& pieceName, Axis axis, float deceleration)
    {

        auto piece = mesh.find(pieceName);
        if (!piece)
        {
            throw std::runtime_error("Invalid piece name: " + pieceName);
        }

        switch (axis)
        {
            case Axis::X:
                setStopSpinOp(piece->get().xTurnOperation, deceleration);
                break;
            case Axis::Y:
                setStopSpinOp(piece->get().yTurnOperation, deceleration);
                break;
            case Axis::Z:
                setStopSpinOp(piece->get().zTurnOperation, deceleration);
                break;
        }
    }

    bool Unit::isMoveInProgress(const std::string& pieceName, Axis axis) const
    {
        auto piece = mesh.find(pieceName);
        if (!piece)
        {
            throw std::runtime_error("Invalid piece name " + pieceName);
        }

        switch (axis)
        {
            case Axis::X:
                return !!(piece->get().xMoveOperation);
            case Axis::Y:
                return !!(piece->get().yMoveOperation);
            case Axis::Z:
                return !!(piece->get().zMoveOperation);
        }

        throw std::logic_error("Invalid axis");
    }

    bool Unit::isTurnInProgress(const std::string& pieceName, Axis axis) const
    {
        auto piece = mesh.find(pieceName);
        if (!piece)
        {
            throw std::runtime_error("Invalid piece name " + pieceName);
        }

        switch (axis)
        {
            case Axis::X:
                return !!(piece->get().xTurnOperation);
            case Axis::Y:
                return !!(piece->get().yTurnOperation);
            case Axis::Z:
                return !!(piece->get().zTurnOperation);
        }

        throw std::logic_error("Invalid axis");
    }

    std::optional<float> Unit::selectionIntersect(const Ray3f& ray) const
    {
        auto line = ray.toLine();
        Line3f modelSpaceLine(line.start - position, line.end - position);
        auto v = selectionMesh.collisionMesh.intersectLine(modelSpaceLine);
        if (!v)
        {
            return std::nullopt;
        }

        return ray.origin.distance(*v);
    }

    bool Unit::isOwnedBy(PlayerId playerId) const
    {
        return owner == playerId;
    }

    bool Unit::isDead() const
    {
        return hitPoints == 0;
    }

    void Unit::markAsDead()
    {
        hitPoints = 0;
    }

    void Unit::clearOrders()
    {
        orders.clear();
        behaviourState = IdleState();

        // not clear if this really belongs here
        clearWeaponTargets();
    }

    void Unit::addOrder(const UnitOrder& order)
    {
        orders.push_back(order);
    }

    class TargetIsUnitVisitor : public boost::static_visitor<bool>
    {
    private:
        UnitId unit;

    public:
        TargetIsUnitVisitor(UnitId unit) : unit(unit) {}
        bool operator()(UnitId target) const { return unit == target; }
        bool operator()(const Vector3f&) const { return false; }
    };

    class IsAttackingUnitVisitor : public boost::static_visitor<bool>
    {
    private:
        UnitId unit;

    public:
        IsAttackingUnitVisitor(const UnitId& unit) : unit(unit) {}
        bool operator()(const UnitWeaponStateIdle&) const { return false; }
        bool operator()(const UnitWeaponStateAttacking& state) const { return boost::apply_visitor(TargetIsUnitVisitor(unit), state.target); }
    };


    class TargetIsPositionVisitor : public boost::static_visitor<bool>
    {
    private:
        Vector3f position;

    public:
        TargetIsPositionVisitor(const Vector3f& position) : position(position) {}
        bool operator()(UnitId) const { return false; }
        bool operator()(const Vector3f& target) const { return target == position; }
    };

    class IsAttackingPositionVisitor : public boost::static_visitor<bool>
    {
    private:
        Vector3f position;

    public:
        IsAttackingPositionVisitor(const Vector3f& position) : position(position) {}
        bool operator()(const UnitWeaponStateIdle&) const { return false; }
        bool operator()(const UnitWeaponStateAttacking& state) const { return boost::apply_visitor(TargetIsPositionVisitor(position), state.target); }
    };

    void Unit::setWeaponTarget(unsigned int weaponIndex, UnitId target)
    {
        auto& weapon = weapons[weaponIndex];
        if (!weapon)
        {
            return;
        }

        if (!boost::apply_visitor(IsAttackingUnitVisitor(target), weapon->state))
        {
            clearWeaponTarget(weaponIndex);
            weapon->state = UnitWeaponStateAttacking(target);
        }
    }

    void Unit::setWeaponTarget(unsigned int weaponIndex, const Vector3f& target)
    {
        auto& weapon = weapons[weaponIndex];
        if (!weapon)
        {
            return;
        }

        if (!boost::apply_visitor(IsAttackingPositionVisitor(target), weapon->state))
        {
            clearWeaponTarget(weaponIndex);
            weapon->state = UnitWeaponStateAttacking(target);
        }
    }

    void Unit::clearWeaponTarget(unsigned int weaponIndex)
    {
        auto& weapon = weapons[weaponIndex];
        if (!weapon)
        {
            return;
        }

        weapon->state = UnitWeaponStateIdle();
        cobEnvironment->createThread("TargetCleared", {static_cast<int>(weaponIndex)});
    }

    void Unit::clearWeaponTargets()
    {
        for (unsigned int i = 0; i < weapons.size(); ++i)
        {
            clearWeaponTarget(i);
        }
    }

    Matrix4f Unit::getTransform() const
    {
        return Matrix4f::translation(position) * Matrix4f::rotationY(rotation);
    }
}
