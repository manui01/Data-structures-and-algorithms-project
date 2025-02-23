// Datastructures.cc
//
// Student name: Manu Ikola

#include "datastructures.hh"

#include <random>
#include <list>
#include <cmath>
#include <iostream>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Write any initialization you need here
}

Datastructures::~Datastructures()
{
    // Write any cleanup you need here
}


unsigned int Datastructures::get_affiliation_count()
{
    unsigned int affiliation_count = affiliations_.size();
    return affiliation_count;
}

void Datastructures::clear_all()
{
    affiliations_.clear();
    publications.clear();
}

std::vector<AffiliationID> Datastructures::get_all_affiliations()
{
    vector<AffiliationID> all_affiliation_ids = {};
    all_affiliation_ids.reserve(affiliations_.size());

    for (auto& element : affiliations_) {
        all_affiliation_ids.push_back(element.first);
    }
    return all_affiliation_ids;
}

bool Datastructures::add_affiliation(AffiliationID id, const Name &name, Coord xy)
{
    auto it = affiliations_.find(id);

    if(it != affiliations_.end()) {
        return false;
    } else {
        Affiliation new_affiliation = {name, xy, {}};
        affiliations_[id] = (new_affiliation);
        return true;
    }
}

Name Datastructures::get_affiliation_name(AffiliationID id)
{
    auto it = affiliations_.find(id);

    if(it != affiliations_.end()) {
        Name affiliation_name = it->second.name;
        return affiliation_name;
    } else {
        return NO_NAME;
    }
}


Coord Datastructures::get_affiliation_coord(AffiliationID id)
{
    auto it = affiliations_.find(id);

    if(it != affiliations_.end()) {
        Coord affiliation_coord = it->second.location;
        return affiliation_coord;
    } else {
        return NO_COORD;
    }
}

std::vector<AffiliationID> Datastructures::get_affiliations_alphabetically()
{
    map<Name, AffiliationID> name_id_map;

    for (auto& element : affiliations_) {
        name_id_map[element.second.name] = element.first;
    }

    vector<AffiliationID> affiliations_all;
    affiliations_all.reserve(affiliations_.size());

    for (auto& element : name_id_map) {
        affiliations_all.push_back(element.second);
    }
    return affiliations_all;
}

std::vector<AffiliationID> Datastructures::get_affiliations_distance_increasing()
{
    vector<pair<Coord, AffiliationID>> coord_id_pair_vec;
    vector<AffiliationID> all_affiliation_ids;
    all_affiliation_ids.reserve(affiliations_.size());

    for (const auto& element : affiliations_) {
        coord_id_pair_vec.push_back({element.second.location,element.first});
    }

    auto compare = [](const auto& pair1, const auto& pair2) {
        const Coord& coord1 = pair1.first;
        const Coord& coord2 = pair2.first;

        double distance1 = sqrt((coord1.x * coord1.x) + (coord1.y * coord1.y));
        double distance2 = sqrt((coord2.x * coord2.x) + (coord2.y * coord2.y));

        if (distance1 == distance2) {
            if (!(coord2 < coord1) && !(coord1 < coord2)) {
                return pair1.second < pair2.second;
            } else {
                return coord1 < coord2;
            }
        } else {
            return distance1 < distance2;
        }
    };

    sort(coord_id_pair_vec.begin(), coord_id_pair_vec.end(), compare);

    for (auto& element : coord_id_pair_vec) {
        all_affiliation_ids.push_back(element.second);
    }

    return all_affiliation_ids;
}

AffiliationID Datastructures::find_affiliation_with_coord(Coord xy)
{
   unordered_map<Coord, AffiliationID, CoordHash> coord_id_map;

   for (auto& element : affiliations_) {
       coord_id_map[element.second.location] = element.first;
   }

   auto it = coord_id_map.find(xy);

   if (it != coord_id_map.end()) {
       return it->second;
   }
   else {
       return NO_AFFILIATION;
   }
}

bool Datastructures::change_affiliation_coord(AffiliationID id, Coord newcoord)
{
    auto it = affiliations_.find(id);

    if(it != affiliations_.end()) {
        it->second.location = newcoord;
        return true;
    } else {
        return false;
    }
}

bool Datastructures::add_publication(PublicationID id, const Name &name, Year year, const std::vector<AffiliationID> &affiliations)
{
    auto it = publications.find(id);

    if(it != publications.end()) {
        return false;
    }
    else {
        Publication new_publication = {name, year, affiliations,{},nullptr};
        publications[id] = new_publication;

        if (affiliations.size() == 0) {
            return true;
        }

        vector<pair<AffiliationID, AffiliationID>> no_prior_connection = {};
        vector<pair<AffiliationID, AffiliationID>> weight_already_updated = {};

        for (const auto& element : affiliations) {
            auto iter = affiliations_.find(element);
            if (iter != affiliations_.end()) {
                iter->second.related_publications.push_back(id);
            }
        }

        // PRG2 STUFF FROM NOW ON

        // Looping all the combinations in affiliations.
        size_t n = affiliations.size();
        for (size_t i=0;i<n-1;i++) {
            for (size_t j=i+1;j<n;j++) {
                AffiliationID first_id = affiliations[i];
                AffiliationID second_id = affiliations[j];
                Connection conn_to_find;
                if (first_id < second_id) {
                    conn_to_find = Connection{first_id, second_id, 1};
                }
                else {
                    conn_to_find = Connection{second_id, first_id, 1};
                }

                // Connection is found from connections if aff1 and aff2 match.
                // weight does not matter.
                auto iter = connections.find(conn_to_find);
                if (iter != connections.end()) {
                    first_id = iter->aff1;
                    second_id = iter->aff2;
                    Weight updated_weight = iter->weight + 1;
                    connections.erase(iter);
                    conn_to_find = Connection{first_id, second_id, updated_weight};
                    connections.insert(conn_to_find);
                }
                else {
                    connections.insert(conn_to_find);
                }

            }
        }
        return true;
    }
}

std::vector<PublicationID> Datastructures::all_publications()
{
    vector<PublicationID> all_publication_ids;
    for (auto& element : publications) {
        all_publication_ids.push_back(element.first);
    }
    return all_publication_ids;
}

Name Datastructures::get_publication_name(PublicationID id)
{
    auto it = publications.find(id);

    if(it != publications.end()) {
        Name publication_name = it->second.title;
        return publication_name;
    } else {
        return NO_NAME;
    }
}

Year Datastructures::get_publication_year(PublicationID id)
{
    auto it = publications.find(id);

    if(it != publications.end()) {
        Year publication_year = it->second.year;
        return publication_year;
    } else {
        return NO_YEAR;
    }
}

std::vector<AffiliationID> Datastructures::get_affiliations(PublicationID id)
{
    std::vector<AffiliationID> all_affiliations;

    auto it = publications.find(id);
    if(it != publications.end()) {
        all_affiliations = it->second.related_affiliations;
        return all_affiliations;
    } else {
        all_affiliations.push_back(NO_AFFILIATION);
        return all_affiliations;
    }
}

bool Datastructures::add_reference(PublicationID id, PublicationID parentid)
{
    auto it = publications.find(id);
    if(it != publications.end()) {
        auto it2 = publications.find(parentid);
        if(it2 != publications.end()) {
            it2->second.references.push_back(id);
            it->second.parent = make_shared<PublicationID>(parentid);
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

std::vector<PublicationID> Datastructures::get_direct_references(PublicationID id)
{
    vector<PublicationID> publication_references;
    auto it = publications.find(id);
    if(it != publications.end()) {
        publication_references = it->second.references;
        return publication_references;
    }
    else {
        publication_references.push_back(NO_PUBLICATION);
        return publication_references;
    }
}

bool Datastructures::add_affiliation_to_publication(AffiliationID affiliationid, PublicationID publicationid)
{
    auto it = affiliations_.find(affiliationid);

    if(it != affiliations_.end()) {
        auto it2 = publications.find(publicationid);
        if (it2 != publications.end()) {
            vector<AffiliationID> aff_connected_to_pub = it2->second.related_affiliations;
            it2->second.related_affiliations.push_back(affiliationid);
            it->second.related_publications.push_back(publicationid);

            vector<PublicationID> vec1 = it->second.related_publications;
            if (aff_connected_to_pub.size() > 0) {
                for (auto& element : aff_connected_to_pub) {

                    auto iter = affiliations_.find(element);
                    vector<PublicationID> vec2 = iter->second.related_publications;

                    Weight w = count_common_elements(vec1, vec2);

                    if (w > 0) {
                        if (affiliationid < element) {
                            Connection new_connection = Connection{affiliationid, element, w};
                            connections.insert(new_connection);
                        }
                        else if (element < affiliationid) {
                            Connection new_connection = Connection{element, affiliationid, w};
                            connections.insert(new_connection);
                        }
                    }
                }
            }
            return true;
        }
        else {
            return false;
        }
    }
    else {
        return false;
    }
}

std::vector<PublicationID> Datastructures::get_publications(AffiliationID id)
{
    vector<PublicationID> all_publications = {};

    auto it = affiliations_.find(id);

    if (it != affiliations_.end()) {
        all_publications = it->second.related_publications;
        return all_publications;
    }
    else {
        all_publications.push_back(NO_PUBLICATION);
        return all_publications;
    }
}

PublicationID Datastructures::get_parent(PublicationID id)
{
    auto it = publications.find(id);

    if (it != publications.end()) {
        auto parent_ptr = it->second.parent;
        if (parent_ptr == nullptr) {
            return NO_PUBLICATION;
        }
        else {
            return *parent_ptr;
        }
    }
    else {
        return NO_PUBLICATION;
    }
}

std::vector<std::pair<Year, PublicationID> > Datastructures::get_publications_after(AffiliationID affiliationid, Year year)
{
    vector<std::pair<Year, PublicationID> > publications_after = {};
    auto it = affiliations_.find(affiliationid);

    if (it != affiliations_.end()) {
        vector<PublicationID> pubs = it->second.related_publications;

        for (auto& element : pubs) {
            Year y = publications[element].year;
            if (y >= year) {

                publications_after.push_back({y, element});
            }
        }

        if (publications_after.size() > 1) {
            sort(publications_after.begin(), publications_after.end());
        }
        return publications_after;

        }
    else {
        publications_after.push_back({NO_YEAR, NO_PUBLICATION});
        return publications_after;
    }
}

std::vector<PublicationID> Datastructures::get_referenced_by_chain(PublicationID id)
{
    vector<PublicationID> publication_chain;
    auto it = publications.find(id);
    if (it != publications.end()) {
        auto parent_ptr = it->second.parent;

        while (parent_ptr != nullptr) {
            publication_chain.push_back(*parent_ptr);
            auto iter = publications.find(*parent_ptr);
            if (iter != publications.end()) {
                parent_ptr = iter->second.parent;
            }
        }
        return publication_chain;
    }
    else {
        publication_chain.push_back(NO_PUBLICATION);
        return publication_chain;
    }
}



// VAPAAVALINTAISET:

std::vector<PublicationID> Datastructures::get_all_references(PublicationID id)
{
    auto it = publications.find(id);

    if (it != publications.end()) {
        std::vector<PublicationID> direct_references = it->second.references;
        std::vector<PublicationID> indirect_references;

        for (const auto element : direct_references) {
            auto iter = publications.find(element);

            if (iter != publications.end()) {
                for (const auto elem : iter->second.references) {
                    indirect_references.push_back(elem);
                }
            }
        }

        direct_references.insert(direct_references.end(), indirect_references.begin(), indirect_references.end());

        return direct_references;
    }
    else {
        std::vector<PublicationID> all_references;
        all_references.push_back(NO_PUBLICATION);
        return all_references;
    }
}



std::vector<AffiliationID> Datastructures::get_affiliations_closest_to(Coord xy)
{
    int x1_coord = xy.x;
    int y1_coord = xy.y;

    map<double, AffiliationID> distance_id_map;

    for (auto& element : affiliations_) {
        int x2_coord = element.second.location.x;
        int y2_coord = element.second.location.y;

        double distance = sqrt((x2_coord-x1_coord)*(x2_coord-x1_coord)+(y2_coord-y1_coord)*(y2_coord-y1_coord));
        distance_id_map[distance] = element.first;
    }

    vector<pair<double, AffiliationID>> distance_id_vec;

    int count = 0;
    for (auto it = distance_id_map.begin(); it != distance_id_map.end() and count < 4; ++it, ++count) {
        pair<double, AffiliationID> distance_id_pair = {it->first, it->second};
        distance_id_vec.push_back(distance_id_pair);
    }

    for (unsigned long long i=1 ; i<distance_id_vec.size() ; i++) {
        double last_distance = distance_id_vec[i-1].first;
        double current_distance = distance_id_vec[i].first;

        if (last_distance == current_distance) {
            AffiliationID last_id = distance_id_vec[i-1].second;
            AffiliationID current_id = distance_id_vec[i].second;

            auto iter1 = affiliations_.find(last_id);
            int y_coord_last = iter1->second.location.y;
            auto iter2 = affiliations_.find(current_id);
            int y_coord_current = iter2->second.location.y;

            // Swapping the elements in distance_id_vec.
            if (y_coord_current < y_coord_last) {
                pair<double, AffiliationID> last_pair = distance_id_vec[i-1];
                pair<double, AffiliationID> current_pair = distance_id_vec[i];
                distance_id_vec[i] = last_pair;
                distance_id_vec[i-1] = current_pair;
            }
        }
    }
    if (distance_id_vec.size() > 3) {
        distance_id_vec.pop_back();
    }

    vector<AffiliationID> closest_affiliations;

    for (auto& element : distance_id_vec) {
        closest_affiliations.push_back(element.second);
    }
    return closest_affiliations;
}

bool Datastructures::remove_affiliation(AffiliationID id)
{
    auto it = affiliations_.find(id);
    if (it != affiliations_.end()) {
        vector<PublicationID> related_publications = it->second.related_publications;
        affiliations_.erase(id);

        // Removing affiliation from publications data.
        for (auto& element : related_publications) {
            auto iter = publications.find(element);
            iter->second.related_affiliations.erase(std::remove(iter->second.related_affiliations.begin()
            , iter->second.related_affiliations.end(), id), iter->second.related_affiliations.end());
        }

        for (auto& connection : connections) {
            if(id == connection.aff1 or id == connection.aff2) {
                connections.erase(connection);
            }
        }

        return true;
    }
    else {
        return false;
    }
}

PublicationID Datastructures::get_closest_common_parent(PublicationID id1, PublicationID id2)
{
    PublicationID closest_common_parent = NO_PUBLICATION;
    auto iter1 = publications.find(id1);

    if (iter1 != publications.end()) {
        auto iter2 = publications.find(id2);
        if (iter2 != publications.end()) {
            vector<PublicationID> id1_reference_chain = get_referenced_by_chain(id1);
            vector<PublicationID> id2_reference_chain = get_referenced_by_chain(id2);

            // Creating vector that contains all the common parent ids.
            vector<PublicationID> common_parents;
            for (auto& element : id1_reference_chain) {
                auto it = find(id2_reference_chain.begin(), id2_reference_chain.end(), element);
                if (it != id2_reference_chain.end()) {
                    common_parents.push_back(element);
                }
            }
            if (common_parents.size() == 1) {
                return common_parents[0];
            }

            else if (common_parents.size() == 0) {
                return closest_common_parent;
            }

            // Finding the closest parent of the common parent vector.
            PublicationID current_id = id1;
            while (true) {
                auto current_parent = publications[current_id].parent;
                auto it = find(common_parents.begin(), common_parents.end(), current_id);
                if (it != common_parents.end()) {
                    return current_id;
                }
                else {
                    current_id = *current_parent;
                }
            }
        }
        else {
            return closest_common_parent;
        }

    }
    else {
        return closest_common_parent;
    }
}

bool Datastructures::remove_publication(PublicationID publicationid)
{
    auto it = publications.find(publicationid);
    if (it != publications.end()) {
        vector<AffiliationID> related_affiliations = it->second.related_affiliations;
        vector<PublicationID> references = it->second.references;
        shared_ptr<PublicationID> parent = it->second.parent;
        publications.erase(publicationid);

        // Removing publication from affiliations data.
        for (auto& element : related_affiliations) {
            auto iter = affiliations_.find(element);
            iter->second.related_publications.erase(std::remove(iter->second.related_publications.begin()
            , iter->second.related_publications.end(), publicationid), iter->second.related_publications.end());
        }

        // Removing publication from its references.
        for (auto& element : references) {
            auto iter = publications.find(element);
            iter->second.parent = nullptr;
        }

        // Removing publication from its parent.
        if (parent != nullptr) {
            auto iter = publications.find(*parent);
            iter->second.references.erase(std::remove(iter->second.references.begin()
            , iter->second.references.end(), publicationid), iter->second.references.end());
        }

        return true;
    }
    else {
        return false;
    }
}


std::vector<Connection> Datastructures::get_connected_affiliations(AffiliationID id)
{

    std::vector<Connection> neighbors;

    for (const auto& connection : connections) {
        if (id == connection.aff1) {
            neighbors.push_back(connection);
        }
        else if (id == connection.aff2) {
            neighbors.push_back(swap_affiliation_ids(connection));
        }
    }
    return neighbors;
}

std::vector<Connection> Datastructures::get_all_connections()
{
    vector<Connection> all_connections = {};
    for (auto& connection : connections) {
        all_connections.push_back(connection);
    }
    return all_connections;
}

Path Datastructures::get_any_path(AffiliationID source, AffiliationID target)
{
    // Crässää jos lähdetään jostain mistä on yhteyksiä ja targettina
    // on jokin johon ei pääse.
    Path current_path;
    std::vector<AffiliationID> visited = {source};
    AffiliationID current_id = source;
    vector<Connection> neighbors = get_connected_affiliations(current_id);
    size_t neighbor_count = neighbors.size();

    if (neighbor_count == 0) {
        current_path = {};
        return current_path;
    }

    size_t count = 0;

    start_search:

    for (const auto& connection : connections) {
        if (connection.aff1 == current_id) {
            if (connection.aff2 == target) {
                current_path.push_back(connection);
                return current_path;
            }
            else {
                auto iter = std::find(visited.begin(), visited.end(), connection.aff2);
                if (iter == visited.end()) {
                    current_id = connection.aff2;
                    current_path.push_back(connection);
                    visited.push_back(current_id);
                    goto start_search;
                }
            }
        }
        else if (connection.aff2 == current_id) {
            if (connection.aff1 == target) {
                current_path.push_back(swap_affiliation_ids(connection));
                return current_path;
            }
            else {
                auto iter = std::find(visited.begin(), visited.end(), connection.aff1);
                if (iter == visited.end()) {
                    current_id = connection.aff1;
                    current_path.push_back(swap_affiliation_ids(connection));
                    visited.push_back(current_id);
                    goto start_search;
                }
            }

        }
    }
    // No next path found.
    if (current_id != source) {

        // Returning back to previous affiliation.
        size_t last_id_index = current_path.size()-1;
        current_id = current_path[last_id_index].aff1;
        current_path.pop_back();
        goto start_search;


        // Just so Qt doesnt give warnings.
        current_path = {};
        return current_path;
    }
    else if (current_id == source and count < neighbor_count-1) {
        count += 1;
        goto start_search;
    }
    else {
        current_path = {};
        return current_path;
    }
}


Weight Datastructures::count_common_elements(const std::vector<PublicationID> vec1, const std::vector<PublicationID> vec2) {
    // Counts how many common elements two vectors has.
    std::unordered_set<PublicationID> set;
    for (const auto elem : vec1) {
        set.insert(elem);
    }

    Weight commonCount = 0;

    for (const auto elem : vec2) {
        if (set.find(elem) != set.end()) {
            commonCount += 1;
        }
    }

    return commonCount;
}

Connection Datastructures::swap_affiliation_ids(Connection conn) {
    // Swaps aff1 and aff2 with each other

    AffiliationID id1 = conn.aff1;
    AffiliationID id2 = conn.aff2;
    Weight weight = conn.weight;

    return Connection{id2, id1, weight};
}
