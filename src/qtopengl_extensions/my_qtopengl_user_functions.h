#ifndef MY_QTOPENGL_USER_FUNCTIONS_H
#define MY_QTOPENGL_USER_FUNCTIONS_H

namespace argos {
   class CMyQtOpenGLUserFunctions;
   class CDebugEntity;
   class CPiPuckExtEntity;
   class CDroneEntity;
   struct SAnchor;
   class QMouseWheelEventHandler;
}

#include <argos3/plugins/simulator/visualizations/qt-opengl/qtopengl_user_functions.h>
#include <argos3/core/utility/datatypes/datatypes.h>

namespace argos {

   class CMyQtOpenGLUserFunctions : public QObject,
                                    public CQTOpenGLUserFunctions {
      Q_OBJECT

   public:
      CMyQtOpenGLUserFunctions();

      virtual ~CMyQtOpenGLUserFunctions() {}

      virtual void Init(TConfigurationNode& t_tree) {}

      void Annotate(CDroneEntity& c_entity);
      void Annotate(CPiPuckExtEntity& c_entity);
      void Annotate(CDebugEntity& c_debug_entity, const SAnchor& s_anchor);

   private:
      void DrawArrow3(const CVector3& c_from, const CVector3& c_to);
      void DrawRing3(const CVector3& c_center, Real f_radius);

   };

   struct CCachedShapes {
      public:
         static CCachedShapes& GetCachedShapes() {
            static CCachedShapes cInstance;
            return cInstance;
         }

         GLuint GetCylinder() const {
            return m_unCylinder;
         }

         GLuint GetCone() const {
            return m_unCone;
         }

         GLuint GetRing() const {
            return m_unRing;
         }

      private:
         CCachedShapes() {
            /* Reserve the needed display lists */
            m_unBaseList = glGenLists(3);
            /* References to the display lists */
            m_unCylinder = m_unBaseList;
            m_unCone     = m_unBaseList + 1;
            m_unRing     = m_unBaseList + 2;
            /* Make cylinder list */
            glNewList(m_unCylinder, GL_COMPILE);
            MakeCylinder();
            glEndList();
            /* Make cone list */
            glNewList(m_unCone, GL_COMPILE);
            MakeCone();
            glEndList();
            /* Make ring list */
            glNewList(m_unRing, GL_COMPILE);
            MakeRing();
            glEndList();
         }

         ~CCachedShapes() {
            glDeleteLists(m_unBaseList, 3);
         }

         void MakeCone();
         void MakeCylinder();
         void MakeRing();
   
         GLuint m_unBaseList;
         GLuint m_unCylinder;
         GLuint m_unCone;
         GLuint m_unRing;
      };
}
#endif
