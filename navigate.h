#ifndef NAVIGATE_H
#define NAVIGATE_H

#include <system_error>
#include "simulation.h"
#include "menu.h"

namespace navigation
{
  void Navigate_main(char pos, simulation::Rocket& rocket) {
    bool keep {true}; // magari c'è una soluzione più elegante ma anche così dovrebbe andare
    while (keep){
    menu::Main_menu main;
    pos = main.Preset_clicked(pos);
    pos= main.Set_clicked(pos);
    pos= main.Play_clicked(pos)
    pos= main.Credits_clicked(pos);
    main.print_main();
    // stampa la finestra del main
    if(pos != 'main'){
      keep = false;
    }
    }
    switch (pos) {

  case 'setting':
  menu::Go_setting(rocket); // chiama una funzione del menu, in quella funzione deve crearsi la schermata dei settaggi
  // proprio come se fosse un secondo main
    break;
  case 'preset':
  menu::Go_preset(rocket);
  case 'credits':
  menu::Go_credits();
    break;
    case 'play':
    // non fa niente, tanto al ciclo dopo nel if se ne accorge e inizia la simulazione
    break;
  default:
    // questa opzione deve essere impossibile, se c'è qualcosa è andato storto
    break;
  }
  void Navigate_setting(char& pos){
    // deve funzionare come il navigate main, copiate il suo codice cambiando il tutto in base a 
    // i vari pulsanti del menu, se trovate soluzioni più valide, molto verosimile, modificate pure
  bool keep {true}; // magari c'è una soluzione più elegante ma anche così dovrebbe andare
    while (keep){
    menu::Setting_menu setting;
    pos = setting.;
    pos= main.Set_clicked(pos);
    pos= main.Play_clicked(pos)
    pos= main.Credits_clicked(pos);
    main.print_main();
    // stampa la finestra del main
    if(pos != 'main'){
      keep = false;
    }
  }
  void Navigate_preset(char& pos){
    // deve funzionare come il navigate main, copiate il suo codice cambiando il tutto in base a 
    // i vari pulsanti del menu, se trovate soluzioni più valide, molto verosimile, modificate pure
    
  
  }

  void Navigate(char& pos, rochet::Rocket rocket;){
  switch (pos)
  {
  case
   'main':
    Navigate_main(pos);
    break;

  case 'setting':
    Navigate_setting(pos);
    break;
  case 'example':
    Navigate_preset(pos);
    break;
  default:
  std::runtime_error;
    break;
  }
  }
} // namespace navigation
}
#endif