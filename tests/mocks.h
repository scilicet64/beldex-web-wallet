#pragma once


#include "../src/MicroCore.h"
#include "../src/CurrentBlockchainStatus.h"
#include "../src/ThreadRAII.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"



namespace
{

//using json = nlohmann::json;
using namespace std;
using namespace cryptonote;
using namespace epee::string_tools;
using namespace std::chrono_literals;

using ::testing::AllOf;
using ::testing::Ge;
using ::testing::Le;
using ::testing::HasSubstr;
using ::testing::Not;
using ::testing::Return;
using ::testing::Throw;
using ::testing::DoAll;
using ::testing::SetArgReferee;
using ::testing::SetArgPointee;
using ::testing::_;
using ::testing::internal::FilePath;



class MockMicroCore : public xmreg::MicroCore
{
public:
    MOCK_METHOD2(init, bool(const string& _blockchain_path,
                            network_type nt));

    MOCK_CONST_METHOD0(get_current_blockchain_height, uint64_t());

    MOCK_CONST_METHOD2(get_block_from_height,
                       bool(uint64_t height, block& blk));

    MOCK_CONST_METHOD2(get_blocks_range,
                       std::vector<block>(const uint64_t& h1,
                                          const uint64_t& h2));

    MOCK_CONST_METHOD3(get_transactions,
                       bool(const std::vector<crypto::hash>& txs_ids,
                            std::vector<transaction>& txs,
                            std::vector<crypto::hash>& missed_txs));

    MOCK_CONST_METHOD1(have_tx, bool(crypto::hash const& tx_hash));

    MOCK_CONST_METHOD1(get_tx_block_height,
                       uint64_t(crypto::hash const& tx_hash));

    MOCK_CONST_METHOD2(tx_exists,
                       bool(crypto::hash const& tx_hash,
                            uint64_t& tx_id));

    MOCK_CONST_METHOD2(get_output_tx_and_index,
                       tx_out_index(uint64_t const& amount,
                                    uint64_t const& index));

    MOCK_CONST_METHOD2(get_tx,
                       bool(crypto::hash const& tx_hash,
                            transaction& tx));

    MOCK_METHOD3(get_output_key,
                    void(const uint64_t& amount,
                         const vector<uint64_t>& absolute_offsets,
                         vector<cryptonote::output_data_t>& outputs));

    MOCK_METHOD2(get_output_key,
                    output_data_t(uint64_t amount,
                                  uint64_t global_amount_index));

    MOCK_CONST_METHOD1(get_tx_amount_output_indices,
                    std::vector<uint64_t>(uint64_t const& tx_id));

    MOCK_CONST_METHOD2(get_random_outs_for_amounts,
                        bool(COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::request const& req,
                             COMMAND_RPC_GET_RANDOM_OUTPUTS_FOR_AMOUNTS::response& res));

    MOCK_CONST_METHOD2(get_outs,
                        bool(const COMMAND_RPC_GET_OUTPUTS_BIN::request& req,
                             COMMAND_RPC_GET_OUTPUTS_BIN::response& res));

    MOCK_CONST_METHOD1(get_dynamic_per_kb_fee_estimate,
                       uint64_t(uint64_t const& grace_blocks));

    MOCK_CONST_METHOD2(get_mempool_txs,
                       bool(vector<tx_info>& tx_infos,
                            vector<spent_key_image_info>& key_image_infos));

      // dont need to mock this function currently
//    MOCK_METHOD3(decrypt_payment_id,
//                       bool(crypto::hash8& payment_id,
//                            crypto::public_key const& public_key,
//                            crypto::secret_key const& secret_key));

};

class MockRPCCalls : public xmreg::RPCCalls
{
public:
    MockRPCCalls(string _deamon_url)
        : xmreg::RPCCalls(_deamon_url)
    {}

    MOCK_METHOD3(commit_tx, bool(const string& tx_blob,
                                 string& error_msg,
                                 bool do_not_relay));
};

class MockTxSearch : public xmreg::TxSearch
{
public:
    MOCK_METHOD0(operator_fcall, void());
    virtual void operator()() {operator_fcall();}

    MOCK_METHOD0(ping, void());

    MOCK_CONST_METHOD0(still_searching, bool());

    MOCK_CONST_METHOD0(get_searched_blk_no, uint64_t());

    MOCK_METHOD0(get_known_outputs_keys,
                 xmreg::TxSearch::known_outputs_t());

    MOCK_CONST_METHOD0(get_xmr_address_viewkey,
                 xmreg::TxSearch::addr_view_t());

    // google mock has some issues with nlohmann::json
    // as input or return parameters in functions it mocks.
    // it resutls in wornings, so to avoid it we are going
    // to proxy json as strings.
    MOCK_METHOD2(mock_find_txs_in_mempool,
                 void(xmreg::TxSearch::pool_txs_t mempool_txs,
                      vector<string>& transactions_json_str));


    // now we need to manualy overwrite TxSearch::find_txs_in_mempool
    // to return j_transactions based on transactions_json_str
    // from the mock_find_txs_in_mempool
    virtual void
    find_txs_in_mempool(pool_txs_t mempool_txs,
                        nlohmann::json* j_transactions)
    {
        vector<string> transactions_json_str;
        mock_find_txs_in_mempool(mempool_txs, transactions_json_str);

        *j_transactions = nlohmann::json::array();

        for (auto& json_str: transactions_json_str)
            j_transactions->push_back(nlohmann::json::parse(json_str));
    }

    MOCK_METHOD0(get_exception_ptr, std::exception_ptr());

};

class MockCurrentBlockchainStatus : public xmreg::CurrentBlockchainStatus
{
public:
    MockCurrentBlockchainStatus()
        : xmreg::CurrentBlockchainStatus(xmreg::BlockchainSetup(),
                                         nullptr, nullptr)
    {}

    MOCK_METHOD3(get_output_keys,
                 bool(const uint64_t& amount,
                      const vector<uint64_t>& absolute_offsets,
                      vector<cryptonote::output_data_t>& outputs));

};
}
