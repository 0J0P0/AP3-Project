int main(int argc, const char *argv[])
{
    if (argc != 3)
    {
        cout << "Two arguments required: input and output file." << endl;
        exit(1);
    }
    ifstream input_file(argv[1],ifstream::in);
    read_input_file(input_file);

    T = 0;
    solution = vector<int>(C, -1);
    auto start = high_resolution_clock::now();  // Get starting timepoint

    exh()

    auto end = high_resolution_clock::now();  // Get ending timepoint
    auto duration = duration_cast<seconds>(end - start);

    ofstream output_file(argv[2],ofstream::out);
    write_output_file(output_file, (double)duration.count());
}