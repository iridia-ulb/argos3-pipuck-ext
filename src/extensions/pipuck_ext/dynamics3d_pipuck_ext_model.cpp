 
#include "dynamics3d_pipuck_ext_model.h"
#include "pipuck_ext_entity.h"

#include <argos3/plugins/simulator/physics_engines/dynamics3d/dynamics3d_engine.h>
#include <argos3/plugins/simulator/physics_engines/dynamics3d/dynamics3d_shape_manager.h>

#include <argos3/plugins/robots/pi-puck/simulator/pipuck_differential_drive_entity.h>

#include <functional>

namespace argos {

   /****************************************/
   /****************************************/

   CDynamics3DPiPuckExtModel::CDynamics3DPiPuckExtModel(CDynamics3DEngine& c_engine,
                                                        CPiPuckExtEntity& c_pipuck) :
      CDynamics3DMultiBodyObjectModel(c_engine, c_pipuck, 4, false),
      m_cDifferentialDriveEntity(c_pipuck.GetDifferentialDriveEntity()) {
      /* get the required collision shapes */
      std::shared_ptr<btCollisionShape> ptrBodyShape =
         CDynamics3DShapeManager::RequestCylinder(m_cBodyHalfExtents);
      std::shared_ptr<btCollisionShape> ptrWheelShape =
         CDynamics3DShapeManager::RequestCylinder(m_cWheelHalfExtents);
      std::shared_ptr<btCollisionShape> ptrMountShape =
         CDynamics3DShapeManager::RequestCylinder(m_cMountHalfExtents);
      /* calculate the inertia of the collision objects */
      btVector3 cBodyInertia;
      btVector3 cWheelInertia;
      btVector3 cMountInertia;
      ptrBodyShape->calculateLocalInertia(m_fBodyMass, cBodyInertia);
      ptrWheelShape->calculateLocalInertia(m_fWheelMass, cWheelInertia);
      ptrMountShape->calculateLocalInertia(m_fMountMass, cMountInertia);
      /* calculate a btTransform that moves us from the global coordinate system to the
         local coordinate system */
      const SAnchor& sOriginAnchor = c_pipuck.GetEmbodiedEntity().GetOriginAnchor();
      const CQuaternion& cOrientation = sOriginAnchor.Orientation;
      const CVector3& cPosition = sOriginAnchor.Position;
      const btTransform& cStartTransform = btTransform(
         btQuaternion(cOrientation.GetX(),
                      cOrientation.GetZ(),
                     -cOrientation.GetY(),
                      cOrientation.GetW()),
         btVector3(cPosition.GetX(),
                   cPosition.GetZ(),
                  -cPosition.GetY()));
      /* create a CAbstractBody::SData structure for each body */
      CAbstractBody::SData sBodyData(
         cStartTransform * m_cBodyOffset,
         m_cBodyGeometricOffset,
         cBodyInertia,
         m_fBodyMass,
         m_fBodyFriction);
      CAbstractBody::SData sLeftWheelData(
         cStartTransform * m_cLeftWheelOffset,
         m_cWheelGeometricOffset,
         cWheelInertia,
         m_fWheelMass,
         m_fWheelFriction);
      CAbstractBody::SData sRightWheelData(
         cStartTransform * m_cRightWheelOffset,
         m_cWheelGeometricOffset,
         cWheelInertia,
         m_fWheelMass,
         m_fWheelFriction);
      CAbstractBody::SData sMountData(
         cStartTransform * m_cMountOffset,
         m_cMountGeometricOffset,
         cMountInertia,
         m_fMountMass,
         m_fMountFriction);
      SAnchor* psBodyAnchor = &c_pipuck.GetEmbodiedEntity().GetAnchor("body");
      SAnchor* psLeftWheelAnchor = &c_pipuck.GetEmbodiedEntity().GetAnchor("left_wheel");
      SAnchor* psRightWheelAnchor = &c_pipuck.GetEmbodiedEntity().GetAnchor("right_wheel");
      /* create the bodies */
      m_ptrBody = std::make_shared<CBase>(*this, psBodyAnchor, ptrBodyShape, sBodyData);
      m_ptrLeftWheel = std::make_shared<CLink>(*this, 0, psLeftWheelAnchor, ptrWheelShape, sLeftWheelData);
      m_ptrRightWheel = std::make_shared<CLink>(*this, 1, psRightWheelAnchor, ptrWheelShape, sRightWheelData);
      m_ptrMount = std::make_shared<CLink>(*this, 2, nullptr, ptrMountShape, sMountData);
      /* copy the bodies to the base class */
      m_vecBodies = {m_ptrBody, m_ptrLeftWheel, m_ptrRightWheel, m_ptrMount};
      /* synchronize with the entity with the space */
      Reset();
   }
   
   /****************************************/
   /****************************************/
   
   void CDynamics3DPiPuckExtModel::Reset() {
      /* reset the base class */
      CDynamics3DMultiBodyObjectModel::Reset();
      /* add mount */
      m_cMultiBody.setupFixed(m_ptrMount->GetIndex(),
                              m_ptrMount->GetData().Mass,
                              m_ptrMount->GetData().Inertia,
                              m_ptrBody->GetIndex(),
                              m_cBodyToMountJointRotation,
                              m_cBodyToMountJointOffset,
                              m_cMountToBodyJointOffset,
                              true);
      /* set up wheels */
      m_cMultiBody.setupRevolute(m_ptrLeftWheel->GetIndex(),
                                 m_ptrLeftWheel->GetData().Mass,
                                 m_ptrLeftWheel->GetData().Inertia,
                                 m_ptrBody->GetIndex(),
                                 m_cBodyToLeftWheelJointRotation,
                                 btVector3(0.0, 1.0, 0.0),
                                 m_cBodyToLeftWheelJointOffset,
                                 m_cLeftWheelToBodyJointOffset,
                                 true);
      m_cMultiBody.setupRevolute(m_ptrRightWheel->GetIndex(),
                                 m_ptrRightWheel->GetData().Mass,
                                 m_ptrRightWheel->GetData().Inertia,
                                 m_ptrBody->GetIndex(),
                                 m_cBodyToRightWheelJointRotation,
                                 btVector3(0.0, 1.0, 0.0),
                                 m_cBodyToRightWheelJointOffset,
                                 m_cRightWheelToBodyJointOffset,
                                 true);
      /* set up motors for the wheels */
      m_ptrLeftMotor = 
         std::make_unique<btMultiBodyJointMotor>(&m_cMultiBody,
                                                 m_ptrLeftWheel->GetIndex(),
                                                 0.0,
                                                 m_fWheelMotorMaxImpulse);
      m_ptrLeftMotor->setRhsClamp(m_fWheelMotorClamp);
      m_ptrRightMotor = 
         std::make_unique<btMultiBodyJointMotor>(&m_cMultiBody,
                                                 m_ptrRightWheel->GetIndex(),
                                                 0.0,
                                                 m_fWheelMotorMaxImpulse);
      m_ptrRightMotor->setRhsClamp(m_fWheelMotorClamp);
      /* Allocate memory and prepare the btMultiBody */
      m_cMultiBody.finalizeMultiDof();
      /* Synchronize with the entity in the space */
      UpdateEntityStatus();
   }

   /****************************************/
   /****************************************/
   
   void CDynamics3DPiPuckExtModel::UpdateEntityStatus() {
      /* write current wheel speeds back to the simulation */
      m_cDifferentialDriveEntity.SetVelocityLeft(m_cMultiBody.getJointVel(m_ptrLeftWheel->GetIndex()) * m_cWheelHalfExtents.getX());
      m_cDifferentialDriveEntity.SetVelocityRight(-m_cMultiBody.getJointVel(m_ptrRightWheel->GetIndex()) * m_cWheelHalfExtents.getX());
      /* run the base class's implementation of this method */
      CDynamics3DMultiBodyObjectModel::UpdateEntityStatus();
   }

   /****************************************/
   /****************************************/

   void CDynamics3DPiPuckExtModel::UpdateFromEntityStatus() {
      /* run the base class's implementation of this method */
      CDynamics3DMultiBodyObjectModel::UpdateFromEntityStatus();
      /* update joint velocities */
      m_ptrLeftMotor->setVelocityTarget(m_cDifferentialDriveEntity.GetTargetVelocityLeft() / m_cWheelHalfExtents.getX(),
                                        m_fWheelMotorKdCoefficient);
      m_ptrRightMotor->setVelocityTarget(-m_cDifferentialDriveEntity.GetTargetVelocityRight() / m_cWheelHalfExtents.getX(),
                                         m_fWheelMotorKdCoefficient);
   }

   /****************************************/
   /****************************************/

   void CDynamics3DPiPuckExtModel::AddToWorld(btMultiBodyDynamicsWorld& c_world) {
      /* run the base class's implementation of this method */
      CDynamics3DMultiBodyObjectModel::AddToWorld(c_world);
      /* add the actuators (btMultiBodyJointMotors) constraints to the world */
      c_world.addMultiBodyConstraint(m_ptrLeftMotor.get());
      c_world.addMultiBodyConstraint(m_ptrRightMotor.get());
   }

   /****************************************/
   /****************************************/

   void CDynamics3DPiPuckExtModel::RemoveFromWorld(btMultiBodyDynamicsWorld& c_world) {
      /* remove the actuators (btMultiBodyJointMotors) constraints from the world */
      c_world.removeMultiBodyConstraint(m_ptrRightMotor.get());
      c_world.removeMultiBodyConstraint(m_ptrLeftMotor.get());
      /* run the base class's implementation of this method */
      CDynamics3DMultiBodyObjectModel::RemoveFromWorld(c_world);
   }

   /****************************************/
   /****************************************/

   REGISTER_STANDARD_DYNAMICS3D_OPERATIONS_ON_ENTITY(CPiPuckExtEntity, CDynamics3DPiPuckExtModel);

   /****************************************/
   /****************************************/

   const btVector3    CDynamics3DPiPuckExtModel::m_cBodyHalfExtents(0.0362, 0.038375, 0.0362);
   const btScalar     CDynamics3DPiPuckExtModel::m_fBodyMass(0.250);
   const btTransform  CDynamics3DPiPuckExtModel::m_cBodyOffset(btQuaternion(0.0, 0.0, 0.0, 1.0), btVector3(0.0,0.00125,0.0));
   const btTransform  CDynamics3DPiPuckExtModel::m_cBodyGeometricOffset(btQuaternion(0.0, 0.0, 0.0, 1.0), btVector3(0.0, -0.038375, 0.0));
   const btVector3    CDynamics3DPiPuckExtModel::m_cWheelHalfExtents(0.02125, 0.0055, 0.02125);
   const btScalar     CDynamics3DPiPuckExtModel::m_fWheelMass(0.1265);
   const btTransform  CDynamics3DPiPuckExtModel::m_cWheelGeometricOffset(btQuaternion(0.0, 0.0, 0.0, 1.0), btVector3(0.0, -0.0015, 0.0));
   const btTransform  CDynamics3DPiPuckExtModel::m_cLeftWheelOffset(btQuaternion(btVector3(-1,0,0), SIMD_HALF_PI), btVector3(0.0, 0.02125, -0.0255));
   const btTransform  CDynamics3DPiPuckExtModel::m_cRightWheelOffset(btQuaternion(btVector3(1,0,0), SIMD_HALF_PI), btVector3(0.0, 0.02125, 0.0255));
   const btVector3    CDynamics3DPiPuckExtModel::m_cBodyToRightWheelJointOffset(0.0, -0.018375, 0.0255);
   const btVector3    CDynamics3DPiPuckExtModel::m_cRightWheelToBodyJointOffset(0.0, 0.0015, 0.0);
   const btQuaternion CDynamics3DPiPuckExtModel::m_cBodyToRightWheelJointRotation(btVector3(-1,0,0), SIMD_HALF_PI);
   const btVector3    CDynamics3DPiPuckExtModel::m_cBodyToLeftWheelJointOffset(0.0, -0.018375, -0.0255);
   const btVector3    CDynamics3DPiPuckExtModel::m_cLeftWheelToBodyJointOffset(0.0, 0.0015, -0.0);
   const btQuaternion CDynamics3DPiPuckExtModel::m_cBodyToLeftWheelJointRotation(btVector3(1,0,0), SIMD_HALF_PI);
   /* TODO calibrate these values */
   const btScalar     CDynamics3DPiPuckExtModel::m_fBodyFriction(0.0);
   const btScalar     CDynamics3DPiPuckExtModel::m_fWheelMotorMaxImpulse(0.05);
   const btScalar     CDynamics3DPiPuckExtModel::m_fWheelFriction(10);
   const btScalar     CDynamics3DPiPuckExtModel::m_fWheelMotorClamp(5.0);
   const btScalar     CDynamics3DPiPuckExtModel::m_fWheelMotorKdCoefficient(2.0);

   /* tag mount */
   const btVector3    CDynamics3DPiPuckExtModel::m_cMountHalfExtents(0.1, 0.0025, 0.1);
   const btScalar     CDynamics3DPiPuckExtModel::m_fMountMass(0.100);
   const btTransform  CDynamics3DPiPuckExtModel::m_cMountOffset(btQuaternion(0.0, 0.0, 0.0, 1.0), btVector3(0.0,0.078,0.0));
   const btTransform  CDynamics3DPiPuckExtModel::m_cMountGeometricOffset(btQuaternion(0.0, 0.0, 0.0, 1.0), btVector3(0.0, -0.0025, 0.0));
   const btScalar     CDynamics3DPiPuckExtModel::m_fMountFriction(0.0);
   const btQuaternion CDynamics3DPiPuckExtModel::m_cBodyToMountJointRotation(0.0, 0.0, 0.0, 1.0);
   const btVector3    CDynamics3DPiPuckExtModel::m_cBodyToMountJointOffset(0.0, 0.038375, 0.0);
   const btVector3    CDynamics3DPiPuckExtModel::m_cMountToBodyJointOffset(0.0, -0.0025, 0.0);

   /****************************************/
   /****************************************/

}
