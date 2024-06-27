

#include "ofApp.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.txt)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//--------------------------------------------------------------
void ofApp::drawGui(){
    //required to call this at beginning
    gui.begin();
    // -------- MENU PRINCIPAL --------
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("|Calibración|"))
        {
            ImGui::NewLine();
            ImGui::SliderFloat("umbral", &threshold, 0.0f, 255.0f); ImGui::SameLine(); HelpMarker("Umbral de brillo para detectar blobs");
            contourFinder.setThreshold(threshold);
            
            ImGui::NewLine();
            ImGui::Separator();
            
            ImGui::Checkbox("fondo adaptativo", &adaptive); ImGui::SameLine(); HelpMarker("fondo adaptativo. El fondo se actualiza según la velocidad de apatación. Si esto está desactivado el fondo es fijo");
            ImGui::NewLine();
            ImGui::SliderFloat("", &adaptSpeed, 0.001f, 0.1f); ImGui::SameLine(); HelpMarker("velocidad a la que se actualiza el fondo. Mayor velocidad se adapta a cambios más rápidos a los cambios del fondo, a costa de una pérdida de presencia de los objetos");
            
            ImGui::NewLine();
            ImGui::Separator();
            ImGui::NewLine();
            if (ImGui::Button("Capturar fondo")){
                 bLearnBackground = true;
            }
            
            ImGui::SameLine();
            
            if (ImGui::Button("borrar fondo")){
                clearBg = true;
            }
            
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("|Blobs|"))
        {
            //ImGui::Checkbox("ventana flotante", &show_bd_config_panel);
            //ImGui::Separator();
            ImGui::NewLine();
            ImGui::SliderFloat("mínimo", &minArea, 0.01f , 50.0f); ImGui::SameLine(); HelpMarker("tamaño mínimo para detedtar un objeto, en porcentaje ");
            ImGui::SliderFloat("máximo", &maxArea, 0.2f, 100.0f); ImGui::SameLine(); HelpMarker("tamaño máximo para detedtar un objeto");
            
            ImGui::NewLine();
            ImGui::Separator();
            ImGui::NewLine();
            ImGui::SliderInt("persistence", &persistence, 0, 100); ImGui::SameLine(); HelpMarker("cantidad de frames que un objeto puede desaparecer antes de ser olvidado por el seguidor");

            ImGui::SliderInt("distance", &distance, 0, 100); ImGui::SameLine(); HelpMarker("cantidad máxima de pixeles que se puede mover para no ser considerado obro objeto");
            
            ImGui::NewLine();
            ImGui::Separator();
            ImGui::NewLine();
            ImGui::Checkbox("Invertir", &invert); ImGui::SameLine(); HelpMarker("busca regiones oscuras en lugar de claras");
            ImGui::Checkbox("Buscar huecos", &findHoles); ImGui::SameLine(); HelpMarker("busca objetos (blobs) dentro de otros");
            ImGui::Separator();
            ImGui::SameLine();

            contourFinder.setMinArea(minArea / 100 * imagePixelNumber);
            contourFinder.setMaxArea(maxArea / 100 * imagePixelNumber);
            contourFinder.setInvert(invert); // find black instead of white
            contourFinder.setFindHoles(findHoles);
            // wait for half a second before forgetting something
            contourFinder.getTracker().setPersistence(persistence);
            // an object can move up to 32 pixels per frame
            contourFinder.getTracker().setMaximumDistance(distance);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("|Entrada|"))
            {
                static const char* item_current = devicesVector[deviceID].c_str();
                if(ImGui::BeginCombo(" ", item_current)){
        
                    for(int i=0; i < devicesVector.size(); ++i){
                        const bool isSelected = (deviceID == i);
                        if(ImGui::Selectable(devicesVector[i].c_str(), isSelected)){
                            deviceID = i;
                            resetCameraSettings(deviceID);
                            item_current = devicesVector[i].c_str();
                        }
                        if(isSelected){
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                ImGui::EndCombo();
                }
                ImGui::SameLine(); HelpMarker("Elegir el dispositivo de entrada");
                
                ImGui::Separator();
                ImGui::NewLine();
                ImGui::Checkbox("ESPEJAR HORIZONTAL", &hMirror);
                ImGui::Checkbox("ESPEJAR VERTICAL", &vMirror);
                
                /*
                ImGui::Separator(); ImGui::Separator();
                
                ImGui::Text("Ajustar perspectiva");
                ImGui::SameLine(); HelpMarker("Ajuste de perspectiva de la imagen de entrada -> seleccionar los puntos con las teclas 1, 2, 3 y 4 ||  mover con el mouse o las flechas del teclado");
                ImGui::Checkbox("habilitar (tecla w)", &warpON);
                ImGui::InputInt("paso", &paso);
                if (ImGui::Button("Resetear perspectiva")){
                     resetWarping = true;
                }
                 */
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("|OSC|"))
        {
            if(ImGui::InputInt("port", &puerto)) sender.setup(host, puerto);
            ImGui::SameLine(); HelpMarker("puerto de conexión");
            
            static char str1[128];
            strcpy(str1, host.c_str());
            //static char str1[128] = "127.0.0.1";
            //ImGui::InputTextWithHint("ip", "enter ip address here", str1, IM_ARRAYSIZE(str1));
            if( ImGui::InputTextWithHint("ip", "enter ip address here",str1, IM_ARRAYSIZE(str1))){
                host = ofVAArgsToString(str1);
                sender.setup(host, puerto);
                //ofLogVerbose() << "--------CAMBIO DE HOST: " << host;
            }
            ImGui::SameLine(); HelpMarker("dirección ip del receptor de mensajes");
            
            ImGui::Separator();
            ImGui::NewLine();
            static char blobsaddress[128];
            strcpy(blobsaddress, etiquetaMensajeBlobs.c_str());
            if( ImGui::InputTextWithHint("address", "tipear etiqueta BLOBS",blobsaddress, IM_ARRAYSIZE(blobsaddress))){
                etiquetaMensajeBlobs = ofVAArgsToString(blobsaddress);
                //ofLogVerbose() << "--------CAMBIO DE ETIQUETA: " << movimientoaddress;
            }
            ImGui::SameLine(); HelpMarker("etiqueta (debe comenzar con /) ");
            
            ImGui::Checkbox("ENVIAR BLOBS", &enviarBlobs);
            ImGui::SameLine(); HelpMarker("habilitar / deshabilitar el envío de blobs");
            
            ImGui::Checkbox("ENVIAR CONTORNOS", &enviarContornos);
            ImGui::SameLine(); HelpMarker("habilitar / deshabilitar el envío de contornos");
            
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("|ver|"))
        {
            ImGui::RadioButton("todas", &imageView, 0); ImGui::SameLine(); HelpMarker("elegirl la imagen que se muestra");
            ImGui::RadioButton("original", &imageView, 1);
            ImGui::RadioButton("escala de grises", &imageView, 2);
            ImGui::RadioButton("fondo", &imageView, 3);
            ImGui::RadioButton("bitonal", &imageView, 4);
            
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("|Acerca|"))
        {
            ImGui::Text("BBlobTracker");
            ImGui::Separator();
            ImGui::Text("Software experimental para captura de movimiento.");
            ImGui::Text("utilizando las técnicas de brillo,sustracción de fondo, ");
            ImGui::Text("para detección de blobs");
            ImGui::Text("Esta aplicación está en desarrollo y no tiene soporte");
            ImGui::Text("..............");
            ImGui::Text("Desarrollado por Matías Romero Costas (Biopus)");
            ImGui::Text("www.biopus.ar");

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    gui.end();
}
