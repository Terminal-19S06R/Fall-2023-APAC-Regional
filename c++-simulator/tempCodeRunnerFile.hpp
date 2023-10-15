            while (!path.empty()) {
                std::vector<int> loc = path.front();
                std::cout << loc[0] << ", " << loc[1] << std::endl;
                path.pop();
            }