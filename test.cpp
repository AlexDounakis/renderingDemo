//
//#include <algorithm>
//#include <array>
//#include <iostream>
//// Collisions   
//
//CollidableNode& hull = *this->m_collidables_nodes[0];
//GeometryNode& terrain = *this->m_nodes[OBJECS::TERRAIN];
//GeometryNode& craft = *this->m_nodes[OBJECS::CRAFT];
//
//// Check for collision between craft and hull    
//
//glm::vec3 craftCenter = glm::vec3(craft.model_matrix[3].x, craft.model_matrix[3].y, craft.model_matrix[3].z);
//glm::vec3 craftNose = craftCenter - glm::vec3(craft.model_matrix[2].x, craft.model_matrix[2].y, craft.model_matrix[2].z);
//float_t isectT = 0.1f;
//if (hull.intersectRay(NodeToCameraCoords(craftCenter), NodeToCameraCoords(craftNose), m_world_matrix, isectT, 1.f))
//{
//    std::cout << "Collision" << std::endl;
//
//}
//
//// Movement     
//// Rotate the craft towards the desired direction    
//// X axis    
//
//float rotationX = 0.f;    
//if (m_turn_right)
//{
//    rotationX -= r;
//}
//if (m_turn_left)
//{
//    rotationX += r;
//}
//
//// Y axis    
//float rotationY = 0.f;
//if (m_turn_up)
//{
//    rotationY += r;
//}
//
//if (m_turn_down)
//{
//    rotationY -= r;
//
//}
//
//if (rotationX != 0.f)
//{
//    craft.model_matrix *= glm::rotate(glm::mat4(1.f), dt * glm::radians(rotationX), glm::vec3(0, 1, 0));
//}    
//
//if (rotationY != 0.f) 
//{ craft.model_matrix *= glm::rotate(glm::mat4(1.f), dt * glm::radians(rotationY), glm::vec3(1, 0, 0)); 
//}     
//
//float boost = 0.f;    
//
//if (m_set_speed) 
//{ 
//    boost = 2 * s; 
//}     
//
//// Move the craft at a constant speed    
//
//glm::vec3 oldPos = glm::vec3(craft.model_matrix[3].x, craft.model_matrix[3].y, craft.model_matrix[3].z);    
//glm::vec3 newPos = oldPos + (s + boost) * dt * glm::vec3(craft.model_matrix[2].x, craft.model_matrix[2].y, craft.model_matrix[2].z);    
//
//craft.model_matrix[3] = glm::vec4(newPos.x, newPos.y, newPos.z, 1);    
//craft.m_aabb.center = glm::vec3(newPos.x, newPos.y, newPos.z);    
//craft.m_aabb.min = glm::vec3(newPos.x, newPos.y - 2, newPos.z);     
//terrain.app_model_matrix = terrain.model_matrix;    
//craft.app_model_matrix = craft.model_matrix;    
//
////craft.RealignAabb();
//
//
//
