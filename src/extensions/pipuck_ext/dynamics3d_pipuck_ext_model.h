#ifndef DYNAMICS3D_PIPUCK_EXT_MODEL_H
#define DYNAMICS3D_PIPUCK_EXT_MODEL_H

namespace argos {
   class CDynamics3DPiPuckExtModel;
   class CPiPuckExtEntity;
   class CPiPuckDifferentialDriveEntity;
}

#include <argos3/plugins/simulator/physics_engines/dynamics3d/bullet/BulletDynamics/Featherstone/btMultiBodyJointMotor.h>
#include <argos3/plugins/simulator/physics_engines/dynamics3d/bullet/BulletDynamics/Featherstone/btMultiBodyJointLimitConstraint.h>
#include <argos3/plugins/simulator/physics_engines/dynamics3d/dynamics3d_multi_body_object_model.h>

namespace argos {

   class CDynamics3DPiPuckExtModel : public CDynamics3DMultiBodyObjectModel {

   public:     

      CDynamics3DPiPuckExtModel(CDynamics3DEngine& c_engine,
                                CPiPuckExtEntity& c_pipuck);

      virtual ~CDynamics3DPiPuckExtModel() {}

      virtual void Reset();

      virtual void UpdateEntityStatus();

      virtual void UpdateFromEntityStatus();

      virtual void AddToWorld(btMultiBodyDynamicsWorld& c_world);

      virtual void RemoveFromWorld(btMultiBodyDynamicsWorld& c_world);

   private:
      /* joint constraints */
      std::unique_ptr<btMultiBodyJointMotor> m_ptrLeftMotor;
      std::unique_ptr<btMultiBodyJointMotor> m_ptrRightMotor;
      /* links */
      std::shared_ptr<CBase> m_ptrBody;
      std::shared_ptr<CLink> m_ptrLeftWheel;
      std::shared_ptr<CLink> m_ptrRightWheel;
      std::shared_ptr<CLink> m_ptrMount;
      /* entities */
      CPiPuckDifferentialDriveEntity& m_cDifferentialDriveEntity;
      /* body data */
      static const btScalar BODY_DISTANCE_FROM_FLOOR;
      static const btVector3 BODY_HALF_EXTENTS;
      static const btScalar BODY_MASS;
      static const btTransform BODY_OFFSET;
      static const btTransform BODY_GEOMETRIC_OFFSET;
      /* wheel data */
      static const btScalar WHEEL_DISTANCE_BETWEEN;
      static const btVector3 WHEEL_HALF_EXTENTS;
      static const btScalar WHEEL_MASS;
      static const btTransform WHEEL_GEOMETRIC_OFFSET;
      static const btTransform WHEEL_OFFSET_LEFT;
      static const btTransform WHEEL_OFFSET_RIGHT;
      static const btVector3 BODY_TO_WHEEL_RIGHT_JOINT_OFFSET;
      static const btVector3 WHEEL_RIGHT_TO_BODY_JOINT_OFFSET;
      static const btQuaternion BODY_TO_WHEEL_RIGHT_JOINT_ROTATION;
      static const btVector3 BODY_TO_WHEEL_LEFT_JOINT_OFFSET;
      static const btVector3 WHEEL_LEFT_TO_BODY_JOINT_OFFSET;
      static const btQuaternion BODY_TO_WHEEL_LEFT_JOINT_ROTATION;
      static const btScalar BODY_FRICTION;
      static const btScalar WHEEL_MOTOR_MAX_IMPULSE;
      static const btScalar WHEEL_FRICTION;
      static const btScalar WHEEL_MOTOR_CLAMP;
      static const btScalar WHEEL_MOTOR_KD_COEFFICIENT;
      /* mount data */
      static const btVector3 MOUNT_HALF_EXTENTS;
      static const btScalar MOUNT_MASS;
      static const btTransform MOUNT_OFFSET;
      static const btTransform MOUNT_GEOMETRIC_OFFSET;
      static const btScalar MOUNT_FRICTION;
      static const btQuaternion BODY_TO_MOUNT_JOINT_ROTATION;
      static const btVector3 BODY_TO_MOUNT_JOINT_OFFSET;
      static const btVector3 MOUNT_TO_BODY_JOINT_OFFSET;
   };
}

#endif
