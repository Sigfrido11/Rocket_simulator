#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "rocket.h"
#include "simulation.h"

// basic sanity checks for advanced engines and orbiting
TEST_CASE("Advanced engine stability and orbiting") {
    // create simple rocket configuration
    std::string name{"test"};
    double mass_structure = 10000.0;
    double up_ar = 1.0;
    double s_p_m = 5000.0;
    double m_s_cont = 1000.0;
    std::vector<double> l_p_m{10000.0};
    std::vector<double> l_c_m{2000.0};
    int n_solid_eng = 1;
    std::vector<int> n_liq_eng{1};

    engine::Ad_sol_engine solid{7e6, 3600.0, 80.0, 0.18, 2.5, 1800.0, 0.00012, 0.35, 22.0/1000.0};
    engine::Ad_liquid_engine liquid{18e6, 3600.0, 0.06, 2.5, 18.0/1000.0};

    rocket::Rocket r{name, mass_structure, up_ar, s_p_m, m_s_cont,
                      l_p_m, l_c_m, &solid, &liquid, n_solid_eng, n_liq_eng};

    // make sure initial state is valid
    CHECK(r.get_pos()[0] > 0);
    CHECK(!std::isnan(r.get_pos()[0]));

    // run a handful of steps ensuring no NaN or negative radius
    Vec thrust{0.0,0.0};
    for (int i=0;i<20;i++) {
        bool orb = rocket::is_orbiting(r.get_pos()[0], r.get_velocity());
        thrust = r.thrust(0.1, 101325.0, orb);
        // simple gravity vector for move
        Vec gravity = rocket::g_force(r.get_pos()[0], r.get_mass(), r.get_velocity()[0]);
        r.move(0.1, thrust);
        CHECK(r.get_pos()[0] > 0);
        CHECK(std::isfinite(r.get_pos()[0]));
        CHECK(std::isfinite(r.get_velocity()[0]));
    }
}
