#ifndef SEED_DB_H
#define SEED_DB_H

#include <string>
#include <deque>

void generate_data_seeding(const std::deque<std::pair<std::string,int> > & ordering);

#endif /* SEED_DB_H */
