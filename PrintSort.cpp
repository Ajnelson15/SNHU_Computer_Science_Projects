//Prints course list alphanumerically
void HashTable::PrintCourseList() {
    
    vector<string> sortedKey;
    string id;

        // Iterate through nodes from begin to end
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {

        // if key not equal to UINT_MAX
        if (it->key != UINT_MAX) {
            id = it->course.courseNumber;
            sortedKey.push_back(id);
            Node* node = it->next;
            while (node != nullptr) {
                node = node->next;
            }
        }
    }
    sort(sortedKey.begin(), sortedKey.end());
    cout << endl;
    for (int i = 0; i < sortedKey.size(); ++i) {
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if (it->key != UINT_MAX) {
                if (sortedKey[i] == it->course.courseNumber) {
                    cout << it->course.courseNumber << ", " << it->course.courseName << endl;


                }
            }
        }

    }cout << endl;
}
