#include <gtest/gtest.h>
#include <faiss/IndexFlat.h>

TEST(Retrieval, SimpleEmbedding) {
    int d = 4;
    faiss::IndexFlatL2 index(d);
    std::vector<float> xb = {1,0,0,0, 0,1,0,0, 0,0,1,0};
    index.add(3, xb.data());
    std::vector<float> xq = {1,0,0,0};
    std::vector<faiss::Index::idx_t> I(1);
    std::vector<float> D(1);
    index.search(1, xq.data(), 1, D.data(), I.data());
    EXPECT_EQ(I[0], 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}